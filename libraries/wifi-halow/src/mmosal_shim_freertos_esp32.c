/*
 * Copyright 2021-2023 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "rom/ets_sys.h"
#include "esp_debug_helpers.h"
#include "esp_private/startup_internal.h"
#include "esp_idf_version.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"

#include "mmosal.h"
#include "mmhal_os.h"

/* --------------------------------------------------------------------------------------------- */

/** Maximum number of failure records to store (must be a power of 2). */
#define MAX_FAILURE_RECORDS 4

/** Fast implementation of _x % _m where _m is a power of 2. */
#define FAST_MOD(_x, _m) ((_x) & ((_m) - 1))

/** Duration to delay before resetting the device on assert. */
#define DELAY_BEFORE_RESET_MS 1000

/** Data structure for assertion information to be preserved. */
struct mmosal_preserved_failure_info
{
    /** Magic number, to check if the info is valid. */
    uint32_t magic;

    /** Number of failures recorded. */
    uint32_t failure_count;

    /** Number of most recently displayed failure. */
    uint32_t displayed_failure_count;

    /** Preserved information from the most recent failure(s). */
    struct mmosal_failure_info info[MAX_FAILURE_RECORDS];
};

/** Magic number to put in @c mmosal_assert_info.magic to indicate that the assertion info
 *  is valid. */
#define ASSERT_INFO_MAGIC (0xabcd1234)

/* Persistent assertion info. Linker script should put this into memory that is not
 * zeroed on boot. Be careful to update linker script if renaming. */
struct mmosal_preserved_failure_info preserved_failure_info __attribute__((section(".noinit")));

void mmosal_log_failure_info(const struct mmosal_failure_info *info)
{
    uint32_t record_num;

    if (preserved_failure_info.magic != ASSERT_INFO_MAGIC)
    {
        preserved_failure_info.failure_count = 0;
        preserved_failure_info.displayed_failure_count = 0;
    }

    preserved_failure_info.magic = ASSERT_INFO_MAGIC;
    record_num = FAST_MOD(preserved_failure_info.failure_count, MAX_FAILURE_RECORDS);
    preserved_failure_info.failure_count++;
    memcpy(&preserved_failure_info.info[record_num], info, sizeof(*info));
}

static void mmosal_dump_failure_info(void)
{
    unsigned first_failure_num = preserved_failure_info.displayed_failure_count;
    unsigned new_failure_count =
        preserved_failure_info.failure_count - preserved_failure_info.displayed_failure_count;
    unsigned failure_offset;

    if (new_failure_count >= MAX_FAILURE_RECORDS)
    {
        first_failure_num = FAST_MOD(preserved_failure_info.failure_count, MAX_FAILURE_RECORDS);
        new_failure_count = MAX_FAILURE_RECORDS;
    }

    for (failure_offset = 0; failure_offset < new_failure_count; failure_offset++)
    {
        unsigned ii;
        unsigned idx = FAST_MOD(first_failure_num + failure_offset, MAX_FAILURE_RECORDS);
        struct mmosal_failure_info *info = &preserved_failure_info.info[idx];

        ets_printf("Failure %u logged at pc 0x%08lx, lr 0x%08lx, line %ld in %08lx\n",
                   first_failure_num + failure_offset,
                   info->pc,
                   info->lr,
                   info->line,
                   info->fileid);

        for (ii = 0; ii < sizeof(info->platform_info) / sizeof(info->platform_info[0]); ii++)
        {
            ets_printf("    0x%08lx\n", info->platform_info[ii]);
        }
    }

    preserved_failure_info.displayed_failure_count = preserved_failure_info.failure_count;
}

void mmosal_impl_assert(void)
{
    ets_printf("MMOSAL Assert, CPU %d (current core) backtrace", xPortGetCoreID());
    (void)esp_backtrace_print(100);
#ifdef HALT_ON_ASSERT
    if (preserved_failure_info.magic == ASSERT_INFO_MAGIC)
    {
        mmosal_dump_failure_info();
    }
    mmosal_disable_interrupts();
    mmhal_log_flush();
    MMPORT_BREAKPOINT();
#else
    mmosal_task_sleep(DELAY_BEFORE_RESET_MS);
    mmhal_reset();
#endif
    while (1)
    {
    }
}

/* Function to be called as part of the secondary initialization. See [System
 * Initialization](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-guides/startup.html#system-initialization)
 * for more information. */
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 3, 0)
ESP_SYSTEM_INIT_FN(mmosal_dump_failure_info, SECONDARY, BIT(0), 999)
#else
ESP_SYSTEM_INIT_FN(mmosal_dump_failure_info, BIT(0), 999)
#endif
{
    if (preserved_failure_info.magic == ASSERT_INFO_MAGIC)
    {
        mmosal_dump_failure_info();
    }
    return ESP_OK;
}

/* --------------------------------------------------------------------------------------------- */

void *mmosal_malloc_(size_t size)
{
    return pvPortMalloc(size);
}

#ifdef MMOSAL_TRACK_ALLOCATIONS
void *mmosal_malloc_dbg(size_t size, const char *name, unsigned line_number)
{
    return pvPortMalloc_dbg(size, name, line_number);
}
#else
void *mmosal_malloc_dbg(size_t size, const char *name, unsigned line_number)
{
    (void)name;
    (void)line_number;
    return pvPortMalloc(size);
}
#endif

void mmosal_free(void *p)
{
    vPortFree(p);
}

void *mmosal_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void *mmosal_calloc(size_t nitems, size_t size)
{
    void *ptr = pvPortMalloc(nitems * size);
    if (ptr == NULL)
    {
        return NULL;
    }

    memset(ptr, 0, nitems * size);
    return ptr;
}

/* --------------------------------------------------------------------------------------------- */

struct mmosal_task_arg
{
    mmosal_task_fn_t task_fn;
    void *task_fn_arg;
};

void mmosal_task_main(void *arg)
{
    struct mmosal_task_arg task_arg = *(struct mmosal_task_arg *)arg;
    mmosal_free(arg);
    task_arg.task_fn(task_arg.task_fn_arg);
    mmosal_task_delete(NULL);
}

struct mmosal_task *mmosal_task_create(mmosal_task_fn_t task_fn,
                                       void *argument,
                                       enum mmosal_task_priority priority,
                                       unsigned stack_size_u32,
                                       const char *name)
{
    TaskHandle_t handle;
    UBaseType_t freertos_priority = tskIDLE_PRIORITY + priority;

    struct mmosal_task_arg *task_arg = (struct mmosal_task_arg *)mmosal_malloc(sizeof(*task_arg));
    if (task_arg == NULL)
    {
        return NULL;
    }
    task_arg->task_fn = task_fn;
    task_arg->task_fn_arg = argument;

    BaseType_t result = xTaskCreate(mmosal_task_main,
                                    name,
                                    stack_size_u32 * 4,
                                    task_arg,
                                    freertos_priority,
                                    &handle);
    if (result == pdFAIL)
    {
        mmosal_free(task_arg);
        return NULL;
    }

    return (struct mmosal_task *)handle;
}

void mmosal_task_delete(struct mmosal_task *task)
{
    vTaskDelete((TaskHandle_t)task);
}

/*
 * Warning: this function should not be used since eTaskGetState() is not a reliable
 * means of testing whether a task has completed.
 *
 * This function will be removed in future.
 */
void mmosal_task_join(struct mmosal_task *task)
{
    while (eTaskGetState((TaskHandle_t)task) != eDeleted)
    {
        mmosal_task_sleep(10);
    }
}

struct mmosal_task *mmosal_task_get_active(void)
{
    return (struct mmosal_task *)xTaskGetCurrentTaskHandle();
}

void mmosal_task_yield(void)
{
    taskYIELD();
}

void mmosal_task_sleep(uint32_t duration_ms)
{
    vTaskDelay(duration_ms / portTICK_PERIOD_MS);
}

static portMUX_TYPE task_spinlock = portMUX_INITIALIZER_UNLOCKED;

void mmosal_task_enter_critical(void)
{
    taskENTER_CRITICAL(&task_spinlock);
}

void mmosal_task_exit_critical(void)
{
    taskEXIT_CRITICAL(&task_spinlock);
}

void mmosal_disable_interrupts(void)
{
    taskDISABLE_INTERRUPTS();
}

void mmosal_enable_interrupts(void)
{
    taskENABLE_INTERRUPTS();
}

const char *mmosal_task_name(void)
{
    TaskHandle_t t = xTaskGetCurrentTaskHandle();
    return pcTaskGetName(t);
}

bool mmosal_task_wait_for_notification(uint32_t timeout_ms)
{
    TickType_t wait = portMAX_DELAY;
    if (timeout_ms < UINT32_MAX)
    {
        wait = pdMS_TO_TICKS(timeout_ms);
    }
    uint32_t ret = ulTaskNotifyTake(pdTRUE, /* Act as binary semaphore */
                                    wait);
    return (ret != 0);
}

void mmosal_task_notify(struct mmosal_task *task)
{
    xTaskNotifyGive((TaskHandle_t)task);
}

void mmosal_task_notify_from_isr(struct mmosal_task *task)
{
    BaseType_t higher_priority_task_woken = pdFALSE;
    vTaskNotifyGiveFromISR((TaskHandle_t)task, &higher_priority_task_woken);
    portYIELD_FROM_ISR(higher_priority_task_woken);
}

/* --------------------------------------------------------------------------------------------- */

struct mmosal_mutex *mmosal_mutex_create(const char *name)
{
    struct mmosal_mutex *mutex = (struct mmosal_mutex *)xSemaphoreCreateMutex();
#if (configUSE_TRACE_FACILITY == 1) && defined(ENABLE_TRACEALYZER) && ENABLE_TRACEALYZER
    if (name != NULL)
    {
        vTraceSetMutexName(mutex, name);
    }
#else
    (void)name;
#endif
    return mutex;
}

void mmosal_mutex_delete(struct mmosal_mutex *mutex)
{
    if (mutex != NULL)
    {
        vQueueDelete((SemaphoreHandle_t)mutex);
    }
}

bool mmosal_mutex_get(struct mmosal_mutex *mutex, uint32_t timeout_ms)
{
    if (mutex == NULL)
        return false;

    SemaphoreHandle_t hMutex = (SemaphoreHandle_t)mutex;
    BaseType_t ret = pdFALSE;

    if (xPortInIsrContext())
    {
        BaseType_t xWoken = pdFALSE;
        ret = xSemaphoreTakeFromISR(hMutex, &xWoken);
        portYIELD_FROM_ISR(xWoken);
    }
    else
    {
        uint32_t timeout_ticks = portMAX_DELAY;
        if (timeout_ms != UINT32_MAX)
        {
            timeout_ticks = timeout_ms / portTICK_PERIOD_MS;
        }
        ret = xSemaphoreTake(hMutex, timeout_ticks);
    }

    return (ret == pdPASS);
}

bool mmosal_mutex_release(struct mmosal_mutex *mutex)
{
    if (mutex == NULL) return pdFALSE;

    SemaphoreHandle_t hMutex = (SemaphoreHandle_t)mutex;
    BaseType_t ret = pdFALSE;

    if (xPortInIsrContext())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        ret = xSemaphoreGiveFromISR(hMutex, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else
    {
        ret = xSemaphoreGive(hMutex);
    }

    return (ret == pdPASS);
}

bool mmosal_mutex_is_held_by_active_task(struct mmosal_mutex *mutex)
{
    return xSemaphoreGetMutexHolder((SemaphoreHandle_t)mutex) == xTaskGetCurrentTaskHandle();
}

/* --------------------------------------------------------------------------------------------- */

struct mmosal_sem *mmosal_sem_create(unsigned max_count, unsigned initial_count, const char *name)
{
    struct mmosal_sem *sem =
        (struct mmosal_sem *)xSemaphoreCreateCounting(max_count, initial_count);
#if (configUSE_TRACE_FACILITY == 1) && defined(ENABLE_TRACEALYZER) && ENABLE_TRACEALYZER
    if (name != NULL)
    {
        vTraceSetSemaphoreName(sem, name);
    }
#else
    (void)name;
#endif
    return sem;
}

void mmosal_sem_delete(struct mmosal_sem *sem)
{
    vQueueDelete((SemaphoreHandle_t)sem);
}

bool mmosal_sem_give(struct mmosal_sem *sem)
{
    if (sem == NULL)
        return false;

    SemaphoreHandle_t hSem = (SemaphoreHandle_t)sem;
    BaseType_t ret;

    if (xPortInIsrContext())
    {
        BaseType_t task_woken = pdFALSE;
        ret = xSemaphoreGiveFromISR(hSem, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    }
    else
    {
        ret = xSemaphoreGive(hSem);
    }
    return (ret == pdPASS);
}

bool mmosal_sem_give_from_isr(struct mmosal_sem *sem)
{
    BaseType_t task_woken = false;
    BaseType_t ret = xSemaphoreGiveFromISR((SemaphoreHandle_t)sem, &task_woken);
    if (ret == pdPASS)
    {
        portYIELD_FROM_ISR(task_woken);
        return true;
    }
    else
    {
        return false;
    }
}

bool mmosal_sem_wait(struct mmosal_sem *sem, uint32_t timeout_ms)
{
    if (sem == NULL)
        return false;

    SemaphoreHandle_t hSem = (SemaphoreHandle_t)sem;
    BaseType_t ret;

    if (xPortInIsrContext())
    {
        BaseType_t task_woken = pdFALSE;
        ret = xSemaphoreTakeFromISR(hSem, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    }
    else
    {
        uint32_t timeout_ticks = portMAX_DELAY;
        if (timeout_ms != UINT32_MAX)
        {
            timeout_ticks = timeout_ms / portTICK_PERIOD_MS;
        }
        ret = xSemaphoreTake(hSem, timeout_ticks);
    }
    return (ret == pdPASS);
}

uint32_t mmosal_sem_get_count(struct mmosal_sem *sem)
{
    return uxSemaphoreGetCount((SemaphoreHandle_t)sem);
}

/* --------------------------------------------------------------------------------------------- */

struct mmosal_semb *mmosal_semb_create(const char *name)
{
    struct mmosal_semb *semb = (struct mmosal_semb *)xSemaphoreCreateBinary();
#if (configUSE_TRACE_FACILITY == 1) && defined(ENABLE_TRACEALYZER) && ENABLE_TRACEALYZER
    if (name != NULL)
    {
        vTraceSetSemaphoreName(semb, name);
    }
#else
    (void)name;
#endif
    return semb;
}

void mmosal_semb_delete(struct mmosal_semb *semb)
{
    vQueueDelete((SemaphoreHandle_t)semb);
}

bool mmosal_semb_give(struct mmosal_semb *semb)
{
    if (semb == NULL)
        return false;

    SemaphoreHandle_t hSem = (SemaphoreHandle_t)semb;
    BaseType_t ret;

    if (xPortInIsrContext())
    {
        BaseType_t task_woken = pdFALSE;
        ret = xSemaphoreGiveFromISR(hSem, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    }
    else
    {
        ret = xSemaphoreGive(hSem);
    }
    return (ret == pdPASS);
}

bool mmosal_semb_give_from_isr(struct mmosal_semb *semb)
{
    BaseType_t task_woken = pdFALSE;
    BaseType_t ret = xSemaphoreGiveFromISR((SemaphoreHandle_t)semb, &task_woken);
    if (ret == pdPASS)
    {
        portYIELD_FROM_ISR(task_woken);
        return true;
    }
    else
    {
        return false;
    }
}

bool mmosal_semb_wait(struct mmosal_semb *semb, uint32_t timeout_ms)
{
    if (semb == NULL)
        return false;

    SemaphoreHandle_t hSem = (SemaphoreHandle_t)semb;
    BaseType_t ret;

    if (xPortInIsrContext())
    {
        BaseType_t task_woken = pdFALSE;
        ret = xSemaphoreTakeFromISR(hSem, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    }
    else
    {
        uint32_t timeout_ticks = portMAX_DELAY;
        if (timeout_ms != UINT32_MAX)
        {
            timeout_ticks = timeout_ms / portTICK_PERIOD_MS;
        }
        ret = xSemaphoreTake(hSem, timeout_ticks);
    }
    return (ret == pdPASS);
}

/* --------------------------------------------------------------------------------------------- */

struct mmosal_queue *mmosal_queue_create(size_t num_items, size_t item_size, const char *name)
{
    struct mmosal_queue *queue = (struct mmosal_queue *)xQueueCreate(num_items, item_size);
#if (configUSE_TRACE_FACILITY == 1) && defined(ENABLE_TRACEALYZER) && ENABLE_TRACEALYZER
    if (name != NULL)
    {
        vTraceSetQueueName(queue, name);
    }
#else
    (void)name;
#endif
    return queue;
}

void mmosal_queue_delete(struct mmosal_queue *queue)
{
    vQueueDelete((SemaphoreHandle_t)queue);
}

bool mmosal_queue_pop(struct mmosal_queue *queue, void *item, uint32_t timeout_ms)
{
    if (queue == NULL || item == NULL)
        return false;

    QueueHandle_t hQueue = (QueueHandle_t)queue;
    BaseType_t ret;

    if (xPortInIsrContext())
    {
        BaseType_t task_woken = pdFALSE;
        ret = xQueueReceiveFromISR(hQueue, item, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    }
    else
    {
        uint32_t timeout_ticks = portMAX_DELAY;
        if (timeout_ms != UINT32_MAX)
        {
            timeout_ticks = timeout_ms / portTICK_PERIOD_MS;
        }
        ret = xQueueReceive(hQueue, item, timeout_ticks);
    }
    return (ret == pdPASS);
}

bool mmosal_queue_push(struct mmosal_queue *queue, const void *item, uint32_t timeout_ms)
{
    if (queue == NULL || item == NULL)
        return false;

    QueueHandle_t hQueue = (QueueHandle_t)queue;
    BaseType_t ret;

    if (xPortInIsrContext())
    {
        BaseType_t task_woken = pdFALSE;
        ret = xQueueSendToBackFromISR(hQueue, item, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    }
    else
    {
        uint32_t timeout_ticks = portMAX_DELAY;
        if (timeout_ms != UINT32_MAX)
        {
            timeout_ticks = timeout_ms / portTICK_PERIOD_MS;
        }
        ret = xQueueSendToBack(hQueue, item, timeout_ticks);
    }
    return (ret == pdPASS);
}

bool mmosal_queue_pop_from_isr(struct mmosal_queue *queue, void *item)
{
    BaseType_t task_woken = pdFALSE;
    if (xQueueReceiveFromISR((SemaphoreHandle_t)queue, item, &task_woken) == pdTRUE)
    {
        portYIELD_FROM_ISR(task_woken);
        return true;
    }
    else
    {
        return false;
    }
}

bool mmosal_queue_push_from_isr(struct mmosal_queue *queue, const void *item)
{
    BaseType_t task_woken = pdFALSE;
    if (xQueueSendToBackFromISR((SemaphoreHandle_t)queue, item, &task_woken) == pdTRUE)
    {
        portYIELD_FROM_ISR(task_woken);
        return true;
    }
    else
    {
        return false;
    }
}

/* --------------------------------------------------------------------------------------------- */

uint32_t mmosal_get_time_ms(void)
{
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

uint32_t mmosal_get_time_ticks(void)
{
    return xTaskGetTickCount();
}

uint32_t mmosal_ticks_per_second(void)
{
    return portTICK_PERIOD_MS * 1000;
}

/* --------------------------------------------------------------------------------------------- */

/**
 * @struct mmosal_timer
 * @brief Structure representing a timer in the MMOSAL (Morse Micro OS Abstraction Layer)
 *
 * This structure encapsulates the necessary information for managing an ESP timer.
 */
struct mmosal_timer
{
    esp_timer_handle_t handle; /**< ESP timer handle. */
    void *arg; /**< User-provided argument to be passed to the callback. */
    timer_callback_t callback; /**< Function to be called when the timer expires. */
    bool auto_reload; /**< If true, the timer will auto restart after expiring. */
    uint64_t period_us; /**< Timer period in microseconds. */
};

static void internal_timer_callback(void *arg)
{
    struct mmosal_timer *timer = (struct mmosal_timer *)arg;
    if (timer && timer->callback)
    {
        timer->callback(timer);
    }
}

struct mmosal_timer *mmosal_timer_create(const char *name,
                                         uint32_t timer_period_ms,
                                         bool auto_reload,
                                         void *arg,
                                         timer_callback_t callback)
{
    esp_timer_create_args_t timer_args = {
        .callback = internal_timer_callback,
        .arg = NULL,
        .name = name,
        .skip_unhandled_events = true,
        .dispatch_method = ESP_TIMER_TASK,
    };

    struct mmosal_timer *timer = mmosal_malloc(sizeof(struct mmosal_timer));
    if (timer == NULL)
    {
        return NULL;
    }

    timer->arg = arg;
    timer->callback = callback;
    timer->auto_reload = auto_reload;
    timer->period_us = timer_period_ms * 1000ULL;
    timer_args.arg = timer;

    if (esp_timer_create(&timer_args, &timer->handle) != ESP_OK)
    {
        mmosal_free(timer);
        return NULL;
    }

    return timer;
}

void mmosal_timer_delete(struct mmosal_timer *timer)
{
    if (timer != NULL)
    {
        esp_timer_stop(timer->handle);
        esp_timer_delete(timer->handle);
        mmosal_free(timer);
    }
}

bool mmosal_timer_start(struct mmosal_timer *timer)
{
    MMOSAL_DEV_ASSERT(timer);

    if (timer->auto_reload)
    {
        return esp_timer_start_periodic(timer->handle, timer->period_us) == ESP_OK;
    }
    else
    {
        return esp_timer_start_once(timer->handle, timer->period_us) == ESP_OK;
    }
}

bool mmosal_timer_stop(struct mmosal_timer *timer)
{
    MMOSAL_DEV_ASSERT(timer);

    return esp_timer_stop(timer->handle) == ESP_OK;
}

bool mmosal_timer_change_period(struct mmosal_timer *timer, uint32_t new_period_ms)
{
    MMOSAL_DEV_ASSERT(timer);

    timer->period_us = new_period_ms * 1000ULL;
    return esp_timer_restart(timer->handle, timer->period_us) == ESP_OK;
}

void *mmosal_timer_get_arg(struct mmosal_timer *timer)
{
    MMOSAL_DEV_ASSERT(timer);

    return timer->arg;
}

bool mmosal_is_timer_active(struct mmosal_timer *timer)
{
    MMOSAL_DEV_ASSERT(timer);

    return esp_timer_is_active(timer->handle);
}

/* --------------------------------------------------------------------------------------------- */

#include <stdarg.h>

int mmosal_printf(const char *format, ...)
{
    int ret;
    va_list args;
    va_start(args, format);

    // 判断是否处于中断上下文
    /*
    if (xPortInIsrContext())
    {
        // 中断：使用 ESP 中断安全串口输出（无锁）
        ret = esp_rom_vprintf(format, args);
    }
    else
    {*/
        // 普通任务：正常走标准 vprintf
        ret = vprintf(format, args);
    //}

    va_end(args);
    return ret;
}
