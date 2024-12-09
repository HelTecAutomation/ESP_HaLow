/*
 * Copyright 2023 Morse Micro
 *
 * This file is licensed under terms that can be found in the LICENSE.md file in the root
 * directory of the Morse Micro IoT SDK software package.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Recommended RX buffer size. */
#define SLIP_RX_BUFFER_SIZE     (2000)

/** Structure used to contain the current state for a slip recieved frame. */
struct slip_rx_state
{
    uint8_t *buffer;        /**< Reference to buffer where processed bytes are recieved. */
    size_t buffer_length;   /**< Length of the buffer. */
    size_t length;          /**< Length of the currently recieved frame, excluding escape bytes. */
    bool escape;            /**< Escape state. */
};

#define SLIP_RX_STATE_INIT(_buffer, _buffer_length) { _buffer, _buffer_length, 0, false }

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
 * Function to send a character on the slip transport.
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
            
            
#ifdef __cplusplus
}
#endif
