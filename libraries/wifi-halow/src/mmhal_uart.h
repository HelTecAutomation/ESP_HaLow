/*
 * Copyright 2024 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @ingroup MMHAL
 * @defgroup MMHAL_UART Morse Micro UART Hardware Abstraction Layer (mmhal_uart) API
 *
 * This provides an abstraction layer for a UART interface. This is used by MM-IoT-SDK example
 * applications.
 *
 * This is a very simple API and leaves UART configuration to the HAL.
 *
 * @{
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "mmosal.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Function type for UART RX callback.
 *
 * @note The UART HAL must not invoke this function from interrupt context. As such, it is safe for
 *       implementations of this callback to assume that it will not be invoked from interrupt
 *       context. However, implementations of this function should avoid blocking for long periods
 *       of time, since this may result in the UART HAL dropping received data if its buffer becomes
 *       full.
 *
 * @param data      The received data.
 * @param length    Length of the received data.
 * @param arg       Opaque argument (as passed in to @ref mmhal_uart_init()).
 */
typedef void (*mmhal_uart_rx_cb_t)(const uint8_t *data, size_t length, void *arg);

/**
 * Initialize the UART HAL and perform any setup necessary.
 *
 * @param rx_cb     Optional callback to be invoked on receive (may be NULL).
 * @param rx_cb_arg Optional opaque argument to be passed to the RX callback. May be NULL.
 */
void mmhal_uart_init(mmhal_uart_rx_cb_t rx_cb, void *rx_cb_arg);

/**
 * Deinitialize the UART HAL, and disable the UART.
 */
void mmhal_uart_deinit(void);

/**
 * Transmit data on the UART. This will block until all data is buffered for transmit (but may
 * return before transmission has completed).
 *
 * @param data      Data to transmit.
 * @param length    Length of @p data.
 */
void mmhal_uart_tx(const uint8_t *data, size_t length);

/** Enumeration of deep sleep modes for the UART HAL. */
enum mmhal_uart_deep_sleep_mode
{
    /** Deep sleep mode is disabled. */
    MMHAL_UART_DEEP_SLEEP_DISABLED,
    /** Enable deep sleep until activity occurs on data-link transport. */
    MMHAL_UART_DEEP_SLEEP_ONE_SHOT,
};

/**
 * Set the deep sleep mode for the UART. See @ref mmhal_uart_deep_sleep_mode for possible deep
 * sleep modes. Note that a given platform may not support all modes.
 *
 * @param mode      The deep sleep mode to set.
 *
 * @returns true if the mode was set successfully; false on failure (e.g., unsupported mode).
 */
bool mmhal_uart_set_deep_sleep_mode(enum mmhal_uart_deep_sleep_mode mode);

#ifdef __cplusplus
}
#endif

/** @} */
