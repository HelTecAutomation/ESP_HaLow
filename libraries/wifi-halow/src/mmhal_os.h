/*
 * Copyright 2025 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @ingroup MMHAL
 * @defgroup MMHAL_OS Morse Micro Operating System Hardware Abstraction Layer (mmhal_os) API
 *
 * This provide an abstraction layer for hardware specific functionality used by the Morse Micro
 * Operating System Abstraction Layer (mmosal) API.
 *
 * @note This API is not used by morselib.
 *
 * @{
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** Initialization before RTOS scheduler starts. */
void mmhal_early_init(void);

/** Initialization after RTOS scheduler started. */
void mmhal_init(void);

/** Reset the microcontroller. */
void mmhal_reset(void);

/** Enumeration of MCU sleep state. */
enum mmhal_sleep_state
{
    MMHAL_SLEEP_DISABLED, /**< Disable MCU sleep. */
    MMHAL_SLEEP_SHALLOW, /**< MCU to enter shallow sleep. */
    MMHAL_SLEEP_DEEP, /**< MCU can enter deep sleep. */
};

/**
 * Function to prepare MCU to enter sleep.
 * This will halt the timer that generates the RTOS tick.
 *
 * @param expected_idle_time_ms Expected time to sleep in milliseconds.
 *
 * @returns the type of sleep permitted by the current system state.
 */
enum mmhal_sleep_state mmhal_sleep_prepare(uint32_t expected_idle_time_ms);

/**
 * Function to enter MCU sleep.
 *
 * @param sleep_state             Sleep state to enter into.
 * @param expected_idle_time_ms   Expected time to sleep in milliseconds.
 *
 * @returns Elapsed sleep time in milliseconds.
 */
uint32_t mmhal_sleep(enum mmhal_sleep_state sleep_state, uint32_t expected_idle_time_ms);

/**
 * Function to abort the MCU sleep state.
 *
 * @note This must only be invoked after @ref mmhal_sleep_prepare()
 *       and before @ref mmhal_sleep().
 *
 * @param sleep_state         Sleep state to abort.
 */
void mmhal_sleep_abort(enum mmhal_sleep_state sleep_state);

/**
 * Function to cleanup on exit from the MCU sleep state.
 */
void mmhal_sleep_cleanup(void);

/** Enumeration of ISR states (i.e., whether in ISR or not). */
enum mmhal_isr_state
{
    MMHAL_NOT_IN_ISR, /**< The function was not executed from ISR context. */
    MMHAL_IN_ISR, /**< The function was executed from ISR context. */
    MMHAL_ISR_STATE_UNKNOWN, /**< The HAL does not support checking ISR state. */
};

/**
 * Get the current ISR state  (i.e., whether in ISR or not).
 *
 * @returns the current ISR state, or @c MMHAL_ISR_STATE_UNKNOWN if the HAL does not support
 *          checking ISR state.
 */
enum mmhal_isr_state mmhal_get_isr_state(void);

/**
 * Write to the debug log.
 *
 * It is assumed the caller will have mechanisms in place to prevent concurrent access.
 *
 * @param data  Buffer containing data to write.
 * @param len   Length of data in buffer.
 */
void mmhal_log_write(const uint8_t *data, size_t len);

/**
 * Flush the debug log before returning.
 *
 * @warning Implementations of this function must support being invoked with interrupts disabled.
 */
void mmhal_log_flush(void);

#ifdef __cplusplus
}
#endif

/** @}  */
