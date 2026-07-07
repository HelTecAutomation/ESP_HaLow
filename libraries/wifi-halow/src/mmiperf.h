/*
 * Copyright 2021-2024 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @ingroup MMAPP
 * @defgroup MMIPERF Morse Micro Iperf
 *
 * This is an iperf implementation that supports both TCP and UDP.
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

/** If a bandwidth limit is set then we divide transmission into blocks and limit the amount of data
 * that can be sent per block. This sets the duration of each block (in milliseconds). */
#define BLOCK_DURATION_MS 200

/** Difference between @c IPv4 and @c IPv6 Header size in bytes. */
#define IPV6_HEADER_SIZE_DIFF (20)

/** Default port for TCP and UDP iperf. */
#define MMIPERF_DEFAULT_PORT (5001)
/** Default packet size for @c IPv4 UDP iperf. */
#define MMIPERF_DEFAULT_UDP_PACKET_SIZE_V4 (1460)
/** Default packet size for @c IPv4 UDP iperf. */
#define MMIPERF_DEFAULT_UDP_PACKET_SIZE_V6 (1440)
/** Default amount iperf (negative number indicates a time in hundredths of seconds, a positive
 *  value indicates a number of bytes). */
#define MMIPERF_DEFAULT_AMOUNT (-1000)
/** Default bandwidth limit for iperf (in kbps) */
#define MMIPERF_DEFAULT_BANDWIDTH (0)

/** Maximum length of an IP address string including null-terminator. */
#define MMIPERF_IPADDR_MAXLEN (48)

#ifndef MMIPERF_STACK_SIZE
/** Default stack to use for MMIPERF tasks. */
#define MMIPERF_STACK_SIZE 512
#endif

/** Enumeration of iperf report types. */
enum mmiperf_report_type
{
    /** The server side test is done */
    MMIPERF_TCP_DONE_SERVER,
    /** The client side test is done */
    MMIPERF_TCP_DONE_CLIENT,
    /** Local error lead to test abort */
    MMIPERF_TCP_ABORTED_LOCAL,
    /** Data check error lead to test abort */
    MMIPERF_TCP_ABORTED_LOCAL_DATAERROR,
    /** Transmit error lead to test abort */
    MMIPERF_TCP_ABORTED_LOCAL_TXERROR,
    /** Remote side aborted the test */
    MMIPERF_TCP_ABORTED_REMOTE,
    /** The server side test is done */
    MMIPERF_UDP_DONE_SERVER,
    /** The client side test is done */
    MMIPERF_UDP_DONE_CLIENT,
    /** Interrim report requested via @ref mmiperf_get_interim_report(). */
    MMIPERF_INTERRIM_REPORT,
};

/** Enumeration of traffic agent state. */
enum traffic_agent_state
{
    /** Traffic agent has not started */
    TRAFFIC_AGENT_NOT_STARTED,
    /** Traffic agent is running */
    TRAFFIC_AGENT_RUNNING,
    /** Traffic agent has stopped */
    TRAFFIC_AGENT_STOPPED,
};

/** Enumeration of Iperf versions. */
enum iperf_version
{
    /** Iperf version 2.0.13 */
    IPERF_VERSION_2_0_13,
    /** Iperf version 2.0.9 */
    IPERF_VERSION_2_0_9,
};

/** Iperf client/server handle. */
typedef struct mmiperf_state *mmiperf_handle_t;

/** Report data structure. */
struct mmiperf_report
{
    /** Type of report. */
    enum mmiperf_report_type report_type;
    /** Local address (as string). */
    char local_addr[MMIPERF_IPADDR_MAXLEN];
    /** Local port. */
    uint16_t local_port;
    /** Remote address (as string). */
    char remote_addr[MMIPERF_IPADDR_MAXLEN];
    /** Remote port. */
    uint16_t remote_port;
    /** Number of bytes of data transferred during test. */
    uint64_t bytes_transferred;
    /** Duration of the test in milliseconds. */
    uint32_t duration_ms;
    /** Average throughput in kbps. */
    uint32_t bandwidth_kbitpsec;
    /** Number of frames transmitted during test (UDP only). */
    uint32_t tx_frames;
    /** Number of frames received during test (UDP only). */
    uint32_t rx_frames;
    /** Number of out of sequence frames received during test (UDP only). */
    uint32_t out_of_sequence_frames;
    /** Number of packet errors (UDP only). */
    uint32_t error_count;
    /** Number of inter-packet gaps (UDP only). */
    uint32_t ipg_count;
    /**
     * Sum of inter-packet gaps (UDP only).
     *
     * @note That this is not strictly the inter-packet gap, but rather the gap between
     *       packet start times.
     */
    uint32_t ipg_sum_ms;
};

/**
 * Report callback function prototype.
 *
 * @param report The report data.
 * @param arg    Opaque argument given when the iperf server/client was started.
 * @param handle Handle of the iperf client/server that generated the report.
 */
typedef void (
    *mmiperf_report_fn)(const struct mmiperf_report *report, void *arg, mmiperf_handle_t handle);

/**
 * Iperf client arguments data structure.
 *
 * For forward compatibility this structure should be initialized using
 * @c MMIPERF_CLIENT_ARGS_DEFAULT. For example:
 *
 * @code{.c}
 * struct mmiperf_client_args args = MMIPERF_CLIENT_ARGS_DEFAULT;
 * @endcode
 */
struct mmiperf_client_args
{
    /** IP address of iperf server to communicate with (as a string). */
    char server_addr[MMIPERF_IPADDR_MAXLEN];
    /** Port on iperf server to communicate with. */
    uint16_t server_port;
    /** Bandwidth limit (in kbps) to communicate with (0 indicates no limit). */
    uint32_t target_bw;
    /** Packet size to use. Only applies to UDP iperf tests.
     *  When set to zero, appropriate value of @c MMIPERF_DEFAULT_UDP_PACKET_SIZE_V4
     *  or @c MMIPERF_DEFAULT_UDP_PACKET_SIZE_V6 will be used
     *  depending on whether the given server_addr is @c IPv4 or @c IPv6 address. */
    uint32_t packet_size;
    /** If positive specifies how many bytes to transfer; if negative the absolute value specifies
     *  the duration of the test in hundredths of seconds. */
    int32_t amount;
    /** Report callback function to invoke on completion/abort. May be @c NULL. */
    mmiperf_report_fn report_fn;
    /** Opaque argument to pass to the report callback. May be @c NULL. */
    void *report_arg;
    /** Iperf version used to parse packet header. */
    enum iperf_version version;
};

/** Initializer for @ref mmiperf_client_args. */
#define MMIPERF_CLIENT_ARGS_DEFAULT                                                              \
    {                                                                                            \
        { 0 }, MMIPERF_DEFAULT_PORT, MMIPERF_DEFAULT_BANDWIDTH, 0, MMIPERF_DEFAULT_AMOUNT, NULL, \
        NULL,  IPERF_VERSION_2_0_13,                                                             \
    }

/**
 * Iperf server arguments data structure.
 *
 * For forward compatibility this structure should be initialized using
 * @c MMIPERF_SERVER_ARGS_DEFAULT. For example:
 *
 * @code{.c}
 * struct mmiperf_server_args args = MMIPERF_SERVER_ARGS_DEFAULT;
 * @endcode
 */
struct mmiperf_server_args
{
    /** Local address to listen on (as a string). */
    char local_addr[MMIPERF_IPADDR_MAXLEN];
    /** Local port to listen on. If zero then @ref MMIPERF_DEFAULT_PORT will be used. */
    uint16_t local_port;
    /** Report callback function to invoke on completion/abort. May be @c NULL. */
    mmiperf_report_fn report_fn;
    /** Opaque argument to pass to the report callback. May be @c NULL. */
    void *report_arg;
    /** Iperf version used to parse packet header. */
    enum iperf_version version;
};

/** Initializer for @ref mmiperf_server_args. */
#define MMIPERF_SERVER_ARGS_DEFAULT                                    \
    {                                                                  \
        { 0 }, MMIPERF_DEFAULT_PORT, NULL, NULL, IPERF_VERSION_2_0_13, \
    }

/**
 * Start a UDP iperf client.
 *
 * @param args Iperf client arguments.
 *
 * @returns a handle to the client on success, or @c NULL on failure.
 */
mmiperf_handle_t mmiperf_start_udp_client(const struct mmiperf_client_args *args);

/**
 * Start a UDP iperf server.
 *
 * @param args Iperf server arguments.
 *
 * @returns a handle to the server on success, or @c NULL on failure.
 */
mmiperf_handle_t mmiperf_start_udp_server(const struct mmiperf_server_args *args);

/**
 * Start a TCP iperf client.
 *
 * @param args Iperf client arguments.
 *
 * @returns a handle to the client on success, or @c NULL on failure.
 */
mmiperf_handle_t mmiperf_start_tcp_client(const struct mmiperf_client_args *args);

/**
 * Start a TCP iperf server.
 *
 * @param args Iperf server arguments.
 *
 * @returns a handle to the server on success, or @c NULL on failure.
 */
mmiperf_handle_t mmiperf_start_tcp_server(const struct mmiperf_server_args *args);

/**
 * Retrieve report for an in progress iperf session.
 *
 * This shall not be called after the report callback has been received for the given
 * session.
 *
 * @param handle Handle to the iperf session to retrieve the report for.
 * @param report Pointer to a report instance to receive the report.
 *
 * @returns @c true on success or @c false on error (e.g., the handle was invalid).
 */
bool mmiperf_get_interim_report(mmiperf_handle_t handle, struct mmiperf_report *report);

#ifdef __cplusplus
}
#endif

/** @} */
