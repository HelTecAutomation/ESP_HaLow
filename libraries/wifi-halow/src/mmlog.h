/*
 * Morse logging API
 *
 * Copyright 2025 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * @defgroup MMLOG Morse Micro Logging Infrastructure
 *
 * Utility macros and functions for outputting log messages.
 *
 * @{
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>

#include "mmosal.h"

/**
 * Macro for printing a @c uint64_t as two separate @c uint32_t values. This is to allow printing
 * of these values even when the @c printf implementation doesn't support it.
 */
#define MM_X64_VAL(value) ((uint32_t)(value >> 32)), ((uint32_t)value)

/** Macro for format specifier to print @ref MM_X64_VAL */
#define MM_X64_FMT "%08lx%08lx"

/**
 * Macro for printing a MAC address. This saves writing it out by hand.
 *
 * Must be used in conjunction with @ref MM_MAC_ADDR_FMT. For example:
 *
 * @code
 * uint8_t mac_addr[] = { 0, 1, 2, 3, 4, 5 };
 * printf("MAC address: " MM_MAC_ADDR_FMT "\n", MM_MAC_ADDR_VAL(mac_addr));
 * @endcode
 */
#define MM_MAC_ADDR_VAL(value) \
    ((value)[0]), ((value)[1]), ((value)[2]), ((value)[3]), ((value)[4]), ((value)[5])

/** Macro for format specifier to print @ref MM_MAC_ADDR_VAL */
#define MM_MAC_ADDR_FMT "%02x:%02x:%02x:%02x:%02x:%02x"

/**
 * Initialize Morse logging API.
 *
 * This should be invoked after OS initialization since it will create a mutex for
 * logging.
 */
void mm_logging_init(void);

/**
 * Dumps a binary buffer in hex.
 *
 * @param level         A single character indicating log level.
 * @param function      Name of function this was invoked from.
 * @param line_number   Line number this was invoked from.
 * @param title         Title of the buffer.
 * @param buf           The buffer to dump.
 * @param len           Length of the buffer.
 */
void mm_hexdump(char level,
                const char *function,
                unsigned line_number,
                const char *title,
                const uint8_t *buf,
                size_t len);

/*
 * The log level is selected as follows, in decending priority order:
 *
 * 1. The value of MMLOG_LEVEL_OVRD (if defined)
 * 2. The value of MMLOG_LEVEL_DEFAULT (if defined)
 * 3. MMLOG_LEVEL_WRN
 */
#if defined(MMLOG_LEVEL_OVRD)
#define MMLOG_LEVEL MMLOG_LEVEL_OVRD
#elif defined(MMLOG_LEVEL_DEFAULT)
#define MMLOG_LEVEL MMLOG_LEVEL_DEFAULT
#else
/** The selected log level. */
#define MMLOG_LEVEL MMLOG_LEVEL_ERR
#endif

/** Invalid log level. */
#define MMLOG_LEVEL_INVALID (0)
/** Logging disabled. */
#define MMLOG_LEVEL_OFF (1)
/** Special log level for always on "application" log messages. These have a special format. */
#define MMLOG_LEVEL_APP (2)
/** Application and error messages only. */
#define MMLOG_LEVEL_ERR (3)
/** Application, error and warning messages only. */
#define MMLOG_LEVEL_WRN (4)
/** Application, error, warning, and info messages. */
#define MMLOG_LEVEL_INF (5)
/** Application, error, warning, info, and debug messages. */
#define MMLOG_LEVEL_DBG (6)
/** Application, error, warning, info, and debug messages, plus additional verbose messages. */
#define MMLOG_LEVEL_VRB (7)

/* ANSI Escape Codes for Colours (0;xx) */
/** ANSI color code: red */
#define MMLOG_COLOR_RED (31)
/** ANSI color code: green */
#define MMLOG_COLOR_GREEN (32)
/** ANSI color code: orange */
#define MMLOG_COLOR_ORANGE (33)
/** ANSI color code: blue */
#define MMLOG_COLOR_BLUE (34)
/** ANSI color code: purple */
#define MMLOG_COLOR_PURPLE (35)
/** ANSI color code: cyan */
#define MMLOG_COLOR_CYAN (36)
/** ANSI color code: light gray */
#define MMLOG_COLOR_LIGHT_GRAY (37)

#if defined(MMLOG_COLOR_ENABLED) && MMLOG_COLOR_ENABLED

/*
 * Prefix/Suffix with Colour Enabled (all log levels excluding APP)
 */
#ifndef MMLOG_PREFIX_FMT
#define MMLOG_PREFIX_FMT "\x1b[0;%um%c %8lu %s %s[%d] "
#endif

#ifndef MMLOG_PREFIX_ARGS
#define MMLOG_PREFIX_ARGS col, lvl, mmosal_get_time_ms(), short_task_name, __func__, __LINE__
#endif

#ifndef MMLOG_SUFFIX_FMT
#define MMLOG_SUFFIX_FMT "\x1b[0m"
#endif

/*
 * Prefix/Suffix with Colour Enabled (APP log level)
 */
#ifndef MMLOG_APP_PREFIX_FMT
#define MMLOG_APP_PREFIX_FMT "\x1b[1;37m  %8lu "
#endif

#ifndef MMLOG_APP_PREFIX_ARGS
#define MMLOG_APP_PREFIX_ARGS mmosal_get_time_ms()
#endif

#ifndef MMLOG_APP_SUFFIX_FMT
#define MMLOG_APP_SUFFIX_FMT "\x1b[0m"
#endif

#else

/*
 * Prefix/Suffix with Colour Disabled (all log levels excluding APP)
 */
#ifndef MMLOG_PREFIX_FMT
/** Log line prefix format string (log levels other than APP). */
#define MMLOG_PREFIX_FMT "%c %8lu %s %s[%d] "
#endif

#ifndef MMLOG_PREFIX_ARGS
/** Log line prefix arguments (log levels other than APP). */
#define MMLOG_PREFIX_ARGS lvl, mmosal_get_time_ms(), short_task_name, __func__, __LINE__
#endif

#ifndef MMLOG_SUFFIX_FMT
/** Log line suffix format string (log levels other than APP). */
#define MMLOG_SUFFIX_FMT
#endif

/*
 * Prefix/Suffix with Colour Enabled (APP messages)
 */
#ifndef MMLOG_APP_PREFIX_FMT
/** Log line prefix format string (APP log level). */
#define MMLOG_APP_PREFIX_FMT "  %8lu "
#endif

#ifndef MMLOG_APP_PREFIX_ARGS
/** Log line prefix arguments (APP log level). */
#define MMLOG_APP_PREFIX_ARGS mmosal_get_time_ms()
#endif

#ifndef MMLOG_APP_SUFFIX_FMT
/** Log line suffix format string (APP log level). */
#define MMLOG_APP_SUFFIX_FMT
#endif

#endif

/**
 * Black hole @c printf to ensure arguments are referenced when a given log level is disabled.
 *
 * @param fmt   The @c printf format string.
 *
 * @returns 0.
 */
static inline int printf_blackhole(const char *fmt, ...)
{
    (void)(fmt);
    return 0;
}

/**
 * Black hole version of @ref mm_hexdump() to ensure arguments are referenced when a
 * given log level is disabled.
 *
 * @param title         Title of the buffer (ignored).
 * @param buf           The buffer to dump (ignored).
 * @param len           Length of the buffer (ignored).
 * */
static inline void mm_hexdump_blackhole(const char *title, const uint8_t *buf, size_t len)
{
    (void)(title);
    (void)(buf);
    (void)(len);
}

#if MMLOG_LEVEL == MMLOG_LEVEL_INVALID
#error Invalid value of MMLOG_LEVEL
#endif

#ifndef MMLOG_PRINTF
/** Logging @c printf definition. */
#define MMLOG_PRINTF(...) mmosal_printf(__VA_ARGS__)
#endif

#if MMLOG_LEVEL >= MMLOG_LEVEL_APP
/** Display an APP level log message. */
#define MMLOG_APP(fmt, ...)                                     \
    MMLOG_PRINTF(MMLOG_APP_PREFIX_FMT fmt MMLOG_APP_SUFFIX_FMT, \
                 MMLOG_APP_PREFIX_ARGS,                         \
                 ##__VA_ARGS__)
#else
#define MMLOG_APP(fmt, ...) printf_blackhole(fmt, ##__VA_ARGS__)
#endif

#if MMLOG_LEVEL >= MMLOG_LEVEL_ERR
/** Generic logging macro. */
#define MMLOG(fmt, _col, _lvl, ...)                                                            \
    do {                                                                                       \
        char lvl = (_lvl);                                                                     \
        char col = (_col);                                                                     \
        char short_task_name[3] = { '?', '?', '\0' };                                          \
        const char *task_name = mmosal_task_name();                                            \
        if (task_name != NULL)                                                                 \
        {                                                                                      \
            short_task_name[0] = task_name[0];                                                 \
            short_task_name[1] = task_name[1];                                                 \
        }                                                                                      \
        (void)(lvl);                                                                           \
        (void)(col);                                                                           \
        (void)(short_task_name);                                                               \
        MMLOG_PRINTF(MMLOG_PREFIX_FMT fmt MMLOG_SUFFIX_FMT, MMLOG_PREFIX_ARGS, ##__VA_ARGS__); \
    } while (0)

/** Display an ERROR level log message. */
#define MMLOG_ERR(fmt, ...) MMLOG(fmt, MMLOG_COLOR_RED, 'E', ##__VA_ARGS__)
/** Dump the given buffer if ERROR level logging is enabled. */
#define MMLOG_DUMP_ERR(title, buf, len) mm_hexdump('E', __func__, __LINE__, (title), (buf), (len))
#else
/** Display an ERROR level log message. */
#define MMLOG_ERR(fmt, ...)             printf_blackhole(fmt, ##__VA_ARGS__)
/** Dump the given buffer if ERROR level logging is enabled. */
#define MMLOG_DUMP_ERR(title, buf, len) mm_hexdump_blackhole(title, buf, len)
#endif

#if MMLOG_LEVEL >= MMLOG_LEVEL_WRN
/** Display an WARNING level log message. */
#define MMLOG_WRN(fmt, ...) MMLOG(fmt, MMLOG_COLOR_ORANGE, 'W', ##__VA_ARGS__)
/** Dump the given buffer if WARNING level logging is enabled. */
#define MMLOG_DUMP_WRN(title, buf, len) mm_hexdump('W', __func__, __LINE__, (title), (buf), (len))
#else
/** Display an WARNING level log message. */
#define MMLOG_WRN(fmt, ...)             printf_blackhole(fmt, ##__VA_ARGS__)
/** Dump the given buffer if WARNING level logging is enabled. */
#define MMLOG_DUMP_WRN(title, buf, len) mm_hexdump_blackhole(title, buf, len)
#endif

#if MMLOG_LEVEL >= MMLOG_LEVEL_INF
/** Display an INFO level log message. */
#define MMLOG_INF(fmt, ...) MMLOG(fmt, MMLOG_COLOR_LIGHT_GRAY, 'I', ##__VA_ARGS__)
/** Dump the given buffer if INFO level logging is enabled. */
#define MMLOG_DUMP_INF(title, buf, len) mm_hexdump('I', __func__, __LINE__, (title), (buf), (len))
#else
/** Display an INFO level log message. */
#define MMLOG_INF(fmt, ...)             printf_blackhole(fmt, ##__VA_ARGS__)
/** Dump the given buffer if INFO level logging is enabled. */
#define MMLOG_DUMP_INF(title, buf, len) mm_hexdump_blackhole(title, buf, len)
#endif

#if MMLOG_LEVEL >= MMLOG_LEVEL_DBG
/** Display an DEBUG level log message. */
#define MMLOG_DBG(fmt, ...) MMLOG(fmt, MMLOG_COLOR_GREEN, 'D', ##__VA_ARGS__)
/** Dump the given buffer if DEBUG level logging is enabled. */
#define MMLOG_DUMP_DBG(title, buf, len) mm_hexdump('D', __func__, __LINE__, (title), (buf), (len))
#else
/** Display an DEBUG level log message. */
#define MMLOG_DBG(fmt, ...)             printf_blackhole(fmt, ##__VA_ARGS__)
/** Dump the given buffer if DEBUG level logging is enabled. */
#define MMLOG_DUMP_DBG(title, buf, len) mm_hexdump_blackhole(title, buf, len)
#endif

#if MMLOG_LEVEL >= MMLOG_LEVEL_VRB
/** Display an VERBOSE level log message. */
#define MMLOG_VRB(fmt, ...) MMLOG(fmt, MMLOG_COLOR_PURPLE, 'V', ##__VA_ARGS__)
/** Dump the given buffer if VERBOSE level logging is enabled. */
#define MMLOG_DUMP_VRB(title, buf, len) mm_hexdump('V', __func__, __LINE__, (title), (buf), (len))
#else
/** Display an VERBOSE level log message. */
#define MMLOG_VRB(fmt, ...)             printf_blackhole(fmt, ##__VA_ARGS__)
/** Dump the given buffer if VERBOSE level logging is enabled. */
#define MMLOG_DUMP_VRB(title, buf, len) mm_hexdump_blackhole(title, buf, len)
#endif

#ifdef __cplusplus
}
#endif

/** @} */
