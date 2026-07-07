/*
 * Copyright 2026 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @defgroup MMVERSION Morse Micro Version API
 *
 * API defining the MM-IoT-SDK version.
 *
 * @sa mmwlan_get_version
 *
 * @{
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>


/**
 * Build identifier string.
 *
 * @note In general, this should not be used directly. Instead use @ref mmwlan_get_version().
 */
#define MM_VERSION_BUILDID  "2.10.4-esp32"

/** Semantic version number major component. */
#define MM_VERSION_MAJOR    (2)
/** Semantic version number minor component. */
#define MM_VERSION_MINOR    (10)
/** Semantic version number patch component . */
#define MM_VERSION_PATCH    (4)

/**
 * Construct a single integer containing a semantic version number with the value
 * `0xMMmmpppp` where:
 *
 * * MM is the major number (8 bits)
 * * mm is the minor number (8 bits)
 * * pppp is the patch number (16 bits)
 */
#define MM_VERSION_NUMBER(_major, _minor, _patch) (((_major) << 24) | ((_minor) << 16) | (_patch))

/**
 * Semantic version number.
 *
 * This can be used along with MM_VERSION_NUMBER to implement version checking using the
 * preprocessor. For example:
 *
 * @code
 * #if MM_VERSION > MM_VERSION_NUMBER(2, 10, 2)
 * // Do something
 * #endif
 * @endcode
 */
#define MM_VERSION  MM_VERSION_NUMBER(MM_VERSION_MAJOR, MM_VERSION_MINOR, MM_VERSION_PATCH)


#ifdef __cplusplus
}
#endif

/** @} */
