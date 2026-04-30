    /*
 * Copyright 2023-2024 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @ingroup MMUTILS
 * @defgroup SLIP Serial Line Internet Protocol (SLIP) implementation.
 *
 * SLIP was originally designed as an encapsulation for IP over serial ports, but can be used
 * for framing of any packet-based data for transmission over a serial port.
 *
 * @{
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/** Recommended RX buffer size. */
#define SLIP_RX_BUFFER_SIZE     (2000)

/**
 * Structure used to contain the current state for the SLIP receiver.
 *
 * For forward compatibility, instances of this structure should be initialized using
 * @ref SLIP_RX_STATE_INIT. For dynamic initialization use @ref slip_rx_state_reinit().
 *
 * For example:
 *
 * @code{.c}
 * // Buffer for SLIP processing on receive path.
 * uint8_t slip_rx_buffer[SLIP_RX_BUFFER_SIZE];
 * // State data for SLIP processing on receive path.
 * struct slip_rx_state slip_rx = SLIP_RX_STATE_INIT(slip_rx_buffer, sizeof(slip_rx_buffer));
 * @endcode
 */
struct slip_rx_state
{
    uint8_t *buffer;        /**< Reference to buffer where processed bytes are received. */
    size_t buffer_length;   /**< Length of the buffer. */
    size_t length;          /**< Length of the currently received frame, excluding escape bytes. */
    bool escape;            /**< Escape state. */
};

/**
 * Static initializer for @ref slip_rx_state.
 *
 * See @ref slip_rx_state for an example of usage.
 *
 * @param _buffer           Pointer to a buffer to be used by SLIP (should be a @c uint8_t array).
 * @param _buffer_length    The size of @c _buffer.
 */
#define SLIP_RX_STATE_INIT(_buffer, _buffer_length) { _buffer, _buffer_length, 0, false }

/**
 * Dynamic (re)initializer for @ref slip_rx_state. This can be used as an alternative to
 * @ref SLIP_RX_STATE_INIT when static initialization is not possible.
 *
 * @param state             The slip state structure to init.
 * @param buffer            Pointer to the buffer to be used by SLIP.
 * @param buffer_length     Length of @p buffer.
 *
 */
static inline void slip_rx_state_reinit(struct slip_rx_state *state,
                                        uint8_t *buffer, size_t buffer_length)
{
    state->buffer = buffer;
    state->buffer_length = buffer_length;
    state->length = 0;
    state->escape = false;
}

/** Enumeration of SLIP status codes. */
enum slip_rx_status
{
    SLIP_RX_COMPLETE,       /**< A complete packet with length > 0 has been received. */
    SLIP_RX_IN_PROGRESS,    /**< Receive is still in progress. */
    SLIP_RX_BUFFER_LIMIT,   /**< Receive buffer limit has been reached. */
    SLIP_RX_ERROR,          /**< An erroneous packet has been received. */
};

/**
 * Handle reception of a character in a SLIP stream.
 *
 * When reception of a packet is successful, this will return @c SLIP_RX_COMPLETE and the
 * packet can be found in `state->buffer` with length `state->length`.
 *
 * @param state Current slip state. Will be updated by this function.
 * @param c     The received character.
 *
 * @return an appropriate value of @ref slip_rx_status.
 */
enum slip_rx_status slip_rx(struct slip_rx_state *state, uint8_t c);

/**
 * Function to send a character on the SLIP transport.
 *
 * @param c     The character to transmit
 * @param arg   Opaque argument, as passed to @c slip_tx().
 *
 * @return 0 on success, otherwise a negative error code.
 */
typedef int (*slip_transport_tx_fn)(uint8_t c, void *arg);

/**
 * Transmit a packet with SLIP framing.
 *
 * @param transport_tx_fn   Function to invoke to send characters on the transport.
 * @param transport_tx_arg  Argument to pass to @p transport_tx_fn.
 * @param packet            The packet to transmit.
 * @param packet_len        The length of the packet.
 *
 * @return 0 on success, otherwise an error code as returned by @p transport_tx_fn.
 */
int slip_tx(slip_transport_tx_fn transport_tx_fn, void *transport_tx_arg,
            const uint8_t *packet, size_t packet_len);

/** @} */
