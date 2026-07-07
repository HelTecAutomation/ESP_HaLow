/*
 * Copyright 2021-2023 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @defgroup MMTRACE Morse Micro  Trace abstraction layer API
 *
 *
 * @{
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** Channel handle */
typedef void *mmtrace_channel;

/**
 * Register a channel.
 *
 * @param name  Channel name.
 *
 * @returns a channel handle.
 */
mmtrace_channel mmtrace_register_channel(const char *name);

/**
 * Log a message on the given channel.
 *
 * @param channel   The channel to log the message on.
 * @param fmt       Print format string.
 */
void mmtrace_printf(mmtrace_channel channel, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

/** \} */
