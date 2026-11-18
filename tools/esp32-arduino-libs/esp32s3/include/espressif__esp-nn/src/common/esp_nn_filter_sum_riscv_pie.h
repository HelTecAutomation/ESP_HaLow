/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Sum of an int8 array as a PIE dot against broadcast ones. Integer addition
 * is associative, so the result is bit-identical to a scalar loop.
 *
 * Shared deliberately, and with history: esp-nn#36 was a scalar form of this
 * sum living privately in fully_connected; the vectorized fix then got
 * re-derived privately (and worse) in the conv path. One PIE home, like
 * esp_nn_filter_sum_esp32s3.h on the S3 side, so twins cannot drift again.
 */
#pragma once

#include <stdint.h>

static inline int32_t esp_nn_filter_sum_s8_riscv_pie(const int8_t *p, int32_t len)
{
    static const int8_t one = 1;
    int32_t sum = 0;
    int32_t idx = 0;

    if (len >= 32) {
        /* double-pumped: two independent q-register streams per iteration */
        const int32_t c32 = (len >> 5) - 1;
        asm volatile (
            "mv x31, %[one]                 \n\t"
            "esp.vldbc.8.ip  q1, x31, 0     \n\t"
            "mv x30, %[inp]                 \n\t"
            "esp.zero.xacc                  \n\t"
            "esp.vld.128.ip  q0, x30, 16    \n\t"
            "esp.vld.128.ip  q2, x30, 16    \n\t"
            "beqz %[c32], 2f                \n\t"
            "mv   s7, %[c32]                \n\t"
            "1:                             \n\t"
            "esp.vmulas.s8.xacc.ld.ip  q0, x30, 16, q0, q1   \n\t"
            "esp.vmulas.s8.xacc.ld.ip  q2, x30, 16, q2, q1   \n\t"
            "addi s7, s7, -1                \n\t"
            "bnez s7, 1b                    \n\t"
            "2:                             \n\t"
            "esp.vmulas.s8.xacc  q0, q1     \n\t"
            "esp.vmulas.s8.xacc  q2, q1     \n\t"
            "esp.movx.r.xacc.l  x29         \n\t"
            "mv %[out], x29                 \n\t"
            : [out] "=r" (sum)
            : [inp] "r" (p), [one] "r" (&one), [c32] "r" (c32)
            : "x29", "x30", "x31", "s7"
        );
        idx = len & ~31;
    } else if (len >= 16) {
        asm volatile (
            "mv x31, %[one]                 \n\t"
            "esp.vldbc.8.ip  q1, x31, 0     \n\t"
            "mv x30, %[inp]                 \n\t"
            "esp.zero.xacc                  \n\t"
            "esp.vld.128.ip  q0, x30, 16    \n\t"
            "esp.vmulas.s8.xacc  q0, q1     \n\t"
            "esp.movx.r.xacc.l  x29         \n\t"
            "mv %[out], x29                 \n\t"
            : [out] "=r" (sum)
            : [inp] "r" (p), [one] "r" (&one)
            : "x29", "x30", "x31"
        );
        idx = 16;
    }
    for (; idx < len; idx++) {
        sum += p[idx];
    }
    return sum;
}
