// SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stdint.h>

/* sum(filter) for the `sum(filter) * input_offset` correction TFLite's
 * asymmetric input quantization requires.
 *
 * Shared deliberately. esp-nn#36 was the scalar form of this in
 * fully_connected; it was fixed there and a perf gate added, and in the same
 * release a fresh scalar copy appeared in convolution because the fix lived in
 * one file rather than in one place. Anything needing this sum should call
 * here, so a new caller gets the vectorized version by default.
 */

static const int8_t esp_nn_fs_one = 1;

/* Sum of `blocks` 16-byte chunks from a 16-byte aligned p, as a dot product
 * against 1s broadcast into q1 - only the data operand is fetched. */
static inline int32_t esp_nn_sum_blocks16_s8_esp32s3(const int8_t *p, int blocks)
{
    int32_t acc;
    asm volatile (
        "ee.zero.accx                          \n"
        "ee.vldbc.8         q1, %[one]         \n"  /* 1s stay in q1 */
        "loopgtz            %[n], .Lens%=      \n"
        "ee.vld.128.ip      q0, %[p], 16       \n"
        "ee.vmulas.s8.accx  q0, q1             \n"
        ".Lens%=:                              \n"
        "nop                                   \n"
        "nop                                   \n"
        "rur.accx_0         %[acc]             \n"
        : [acc] "=r" (acc), [p] "+r" (p), [n] "+r" (blocks)
        : [one] "r" (&esp_nn_fs_one)
        : "memory"
    );
    return acc;
}

/* Summing is order independent, so walk to the next 16-byte boundary in scalar
 * and vectorize the rest: ee.vld.128 needs the alignment, and this way nothing
 * is read past p + len. */
static inline int32_t esp_nn_filter_sum_s8_esp32s3(const int8_t *p, int len)
{
    int32_t sum = 0;
    int i = 0;

    while (i < len && (((uintptr_t)(p + i)) & 15)) {
        sum += p[i++];
    }
    const int blocks = (len - i) >> 4;
    if (blocks > 0) {
        sum += esp_nn_sum_blocks16_s8_esp32s3(p + i, blocks);
        i += blocks << 4;
    }
    for (; i < len; i++) {
        sum += p[i];
    }
    return sum;
}
