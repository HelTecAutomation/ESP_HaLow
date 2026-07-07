/*
 * Copyright 2025 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @ingroup MMHAL
 * @defgroup MMHAL_CORE Morse Micro Hardware Abstraction Layer Core (mmhal_core) API
 *
 * These are basic HAL functions required for the core Morse library to operate.
 *
 * @{
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Generate a random 32 bit integer within the given range.
 *
 * @param min   Minimum value (inclusive).
 * @param max   Maximum value (inclusive).
 *
 * @returns a randomly generated integer (min <= i <= max).
 */
uint32_t mmhal_random_u32(uint32_t min, uint32_t max);

/** Enumeration of veto_id ranges for use with @ref mmhal_set_deep_sleep_veto() and
 *  @ref mmhal_clear_deep_sleep_veto(). */
enum mmhal_veto_id
{
    /** Start of deep sleep veto ID range that is available for application use. */
    MMHAL_VETO_ID_APP_MIN = 0,
    /** End of deep sleep veto ID range that is available for application use. */
    MMHAL_VETO_ID_APP_MAX = 7,
    /** Start of deep sleep veto ID range that is available for HAL use. */
    MMHAL_VETO_ID_HAL_MIN = 8,
    /** End of deep sleep veto ID range that is available for HAL use. */
    MMHAL_VETO_ID_HAL_MAX = 15,
    /** Start of deep sleep veto ID range that is allocated for morselib use. Note that this
     *  must not be changed as it is built into morselib. */
    MMHAL_VETO_ID_MORSELIB_MIN = 16,
    /** End of deep sleep veto ID range that is allocated for morselib use. Note that this must not
     *  be changed as it is built into morselib. */
    MMHAL_VETO_ID_MORSELIB_MAX = 19,
    /** Deep sleep veto ID for data-link subsystem. */
    MMHAL_VETO_ID_DATALINK = 20,
    /** Deep sleep veto ID allocated to @ref MMCONFIG. */
    MMHAL_VETO_ID_MMCONFIG = 21,
    /** Start of deep sleep veto ID range reserved for future use. */
    MMHAL_VETO_ID_RESERVED_MIN = 22,
    /** End of deep sleep veto ID range reserved for future use. */
    MMHAL_VETO_ID_RESERVED_MAX = 31,
};

/**
 * Sets a deep sleep veto that will prevent the device from entering deep sleep. The device
 * will not enter deep sleep until there are no vetoes remaining. This veto can be cleared
 * by a call to @ref mmhal_clear_deep_sleep_veto() with the same veto_id.
 *
 * Up to 32 vetoes are supported (ID numbers 0-31). Each veto should be used exclusively by
 * a given aspect of the system (e.g., to prevent deep sleep when a DMA transfer is in progress,
 * or to prevent deep sleep when there is log data buffered for transmit, etc.).
 *
 * @param veto_id   The veto identifier. Valid values are 0-31, and these are split up into ranges
 *                  for use by different subsystems -- see @ref mmhal_veto_id.
 */
void mmhal_set_deep_sleep_veto(uint8_t veto_id);

/**
 * Clears a deep sleep veto that was preventing the device from entering deep sleep (see
 * @ref mmhal_set_deep_sleep_veto()). If the given veto was not already set then this has
 * no effect.
 *
 * @param veto_id   The veto identifier. Valid values are 0-31, and these are split up into ranges
 *                  for use by different subsystems -- see @ref mmhal_veto_id.
 */
void mmhal_clear_deep_sleep_veto(uint8_t veto_id);

#ifdef __cplusplus
}
#endif

/** @} */
