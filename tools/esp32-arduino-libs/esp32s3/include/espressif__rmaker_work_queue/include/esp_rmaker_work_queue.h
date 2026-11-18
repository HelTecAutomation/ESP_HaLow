/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <esp_err.h>
#include <sdkconfig.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#ifdef __cplusplus
extern "C"
{
#endif
/** Prototype for ESP RainMaker Work Queue Function
 *
 * @param[in] priv_data The private data associated with the work function.
 */
typedef void (*esp_rmaker_work_fn_t)(void *priv_data);

/* ---- Using the global Work Queue ---- */

/** Initializes the Work Queue
 *
 * This initializes the work queue, which is basically a mechanism to run
 * tasks in the context of a dedicated thread. You can start queueing tasks
 * after this, but they will get executed only after calling
 * esp_rmaker_work_queue_start().
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t esp_rmaker_work_queue_init(void);

/** De-initialize the Work Queue
 *
 * This de-initializes the work queue. Note that the work queue needs to
 * be stopped using esp_rmaker_work_queue_stop() before calling this.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t esp_rmaker_work_queue_deinit(void);

/** Start the Work Queue
 *
 * This starts the Work Queue thread which then starts executing the tasks queued.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t esp_rmaker_work_queue_start(void);

/** Stop the Work Queue
 *
 * This stops a running Work Queue.
 *
 * @note The worker stack and TCB stay allocated until esp_rmaker_work_queue_deinit().
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t esp_rmaker_work_queue_stop(void);

/** Queue execution of a function in the Work Queue's context
 *
 * This API queues a work function for execution in the Work Queue Task's context.
 *
 * @param[in] work_fn The Work function to be queued. Must not be NULL.
 * @param[in] priv_data Private data to be passed to the work function.
 *
 * @return ESP_OK on success.
 * @return ESP_ERR_INVALID_ARG if work_fn is NULL.
 * @return error in case of failure.
 */
esp_err_t esp_rmaker_work_queue_add_task(esp_rmaker_work_fn_t work_fn, void *priv_data);

/* ---- Using a Work Queue instance ---- */

/** Handle for a Work Queue instance, separate from the global Work Queue */
typedef struct esp_rmaker_work_queue *esp_rmaker_work_queue_handle_t;

/** Configuration for a Work Queue instance
 *
 * @note Start from ESP_RMAKER_WORK_QUEUE_CONFIG_DEFAULT() and override fields, so new fields get defaults.
 */
typedef struct {
    uint32_t size;              /* Max number of pending entries */
    uint32_t stack_size;        /* Task stack size in bytes */
    uint32_t priority;          /* Task priority, 1 to configMAX_PRIORITIES - 1 (0 is rejected) */
    bool prefer_ext_ram;        /* Whether to prefer external RAM for the task stack */
    const char *name;           /* Task name, copied at init; NULL gives "rmaker_wq-<n>" */
    BaseType_t core_id;         /* Core to pin the task to, or tskNO_AFFINITY */
} esp_rmaker_work_queue_config_t;

/** Default configuration for a Work Queue instance
 *
 * Follows the sdkconfig defaults.
 */
#ifdef CONFIG_ESP_RMAKER_WORK_QUEUE_PREFER_EXT_RAM
#define ESP_RMAKER_WORK_QUEUE_PREFER_EXT_RAM_DEFAULT true
#else
#define ESP_RMAKER_WORK_QUEUE_PREFER_EXT_RAM_DEFAULT false
#endif

#define ESP_RMAKER_WORK_QUEUE_CONFIG_DEFAULT() { \
    .size = CONFIG_ESP_RMAKER_WORK_QUEUE_SIZE, \
    .stack_size = CONFIG_ESP_RMAKER_WORK_QUEUE_TASK_STACK, \
    .priority = CONFIG_ESP_RMAKER_WORK_QUEUE_TASK_PRIORITY, \
    .prefer_ext_ram = ESP_RMAKER_WORK_QUEUE_PREFER_EXT_RAM_DEFAULT, \
    .name = NULL, \
    .core_id = tskNO_AFFINITY, \
}

/** Initialize a Work Queue instance
 *
 * This initializes a Work Queue instance.
 *
 * @param[out] p_handle The pointer to the handle for the Work Queue. Must point to NULL.
 * @param[in] p_config The configuration for the Work Queue.
 *
 * @return ESP_OK on success.
 * @return ESP_ERR_INVALID_STATE if *p_handle is not NULL.
 * @return error in case of failure.
 */
esp_err_t esp_rmaker_work_queue_init_inst(esp_rmaker_work_queue_handle_t *p_handle, const esp_rmaker_work_queue_config_t *p_config);

/** De-initialize a Work Queue instance
 *
 * This de-initializes a Work Queue instance.
 *
 * @param[in,out] p_handle The handle for the Work Queue. Will be set to NULL on success.
 *
 * @note Do not call this from a work function of the same queue.
 *
 * @return ESP_OK on success.
 * @return ESP_ERR_INVALID_STATE if called from the queue's own worker.
 * @return error in case of failure.
 */
esp_err_t esp_rmaker_work_queue_deinit_inst(esp_rmaker_work_queue_handle_t *p_handle);

/** Start a Work Queue instance
 *
 * This starts a Work Queue instance.
 *
 * @param[in] handle The handle for the Work Queue.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t esp_rmaker_work_queue_start_inst(esp_rmaker_work_queue_handle_t handle);

/** Stop a Work Queue instance
 *
 * This stops a Work Queue instance.
 *
 * @note The worker stack and TCB stay allocated until esp_rmaker_work_queue_deinit_inst().
 *
 * @param[in] handle The handle for the Work Queue.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t esp_rmaker_work_queue_stop_inst(esp_rmaker_work_queue_handle_t handle);

/** Queue execution of a function in the Work Queue instance's context
 *
 * This API queues a work function for execution in the Work Queue instance's task context.
 *
 * @param[in] handle The handle for the Work Queue.
 * @param[in] work_fn The Work function to be queued. Must not be NULL.
 * @param[in] priv_data Private data to be passed to the work function.
 *
 * @return ESP_OK on success.
 * @return ESP_ERR_INVALID_ARG if handle or work_fn is NULL.
 * @return error in case of failure.
 */
esp_err_t esp_rmaker_work_queue_add_task_inst(esp_rmaker_work_queue_handle_t handle, esp_rmaker_work_fn_t work_fn, void *priv_data);

#ifdef __cplusplus
}
#endif
