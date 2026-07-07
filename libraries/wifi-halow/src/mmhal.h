/*
 * Copyright 2021-2023 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* clang-format off */
/**
 * @defgroup MMHAL Morse Micro Hardware Abstraction Layer (mmhal) API
 *
 * This API provides abstraction from the underlying hardware/BSP. It is broken up into several
 * files for modularity. As such, not all HALs may need to be implemented in all cases.
 *
 * | Header File        | Description      | Required for... |
 * |--------------------|------------------|-----------------|
 * | @ref mmhal_core.h  | @ref MMHAL_CORE  | Required for core morselib functionality. |
 * | @ref mmhal_wlan.h  | @ref MMHAL_WLAN  | Required for @ref MMWLAN functionality. |
 * | @ref mmhal_os.h    | @ref MMHAL_OS    | Required for the @ref MMOSAL provided with this SDK. |
 * | @ref mmhal_app.h   | @ref MMHAL_APP   | Required for the @ref MMAPPS provided with this SDK. |
 * | @ref mmhal_uart.h  | @ref MMHAL_UART  | Required for certain example applications that need UART I/O. |
 * | @ref mmhal_flash.h | @ref MMHAL_FLASH | Required for @ref MMCONFIG, @c littlefs HALs, and certain example applications. |
 *
 */
/* clang-format on */

#pragma once

#include "mmhal_core.h"
#include "mmhal_wlan.h"
#include "mmhal_os.h"
#include "mmhal_app.h"
#include "mmhal_flash.h"
