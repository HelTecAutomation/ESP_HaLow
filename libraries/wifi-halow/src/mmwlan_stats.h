/*
 * Copyright 2021-2025 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Warning: this file is auto-generated. Do not modify by hand.
 */

/* */

#pragma once

/**
 * @ingroup MMWLAN_STATS
 * @{
 *
 * @defgroup MMWLAN_UMAC_STATS UMAC Stats
 *
 * API to get stats from the UMAC.
 *
 * @{
 */

/**
 * Enumeration of valid indexes for connect_timestamp.
 */
enum mmwlan_stats_connect_timestamp_index
{
    MMWLAN_STATS_CONNECT_TIMESTAMP_START,
    MMWLAN_STATS_CONNECT_TIMESTAMP_SCAN_REQUESTED,
    MMWLAN_STATS_CONNECT_TIMESTAMP_SCAN_COMPLETE,
    MMWLAN_STATS_CONNECT_TIMESTAMP_SEND_AUTH_1,
    MMWLAN_STATS_CONNECT_TIMESTAMP_RECV_AUTH_1,
    MMWLAN_STATS_CONNECT_TIMESTAMP_SEND_AUTH_2,
    MMWLAN_STATS_CONNECT_TIMESTAMP_RECV_AUTH_2,
    MMWLAN_STATS_CONNECT_TIMESTAMP_SEND_ASSOC,
    MMWLAN_STATS_CONNECT_TIMESTAMP_RECV_ASSOC,
    MMWLAN_STATS_CONNECT_TIMESTAMP_LINK_UP,
    MMWLAN_STATS_CONNECT_TIMESTAMP_N_ENTRIES
};

/**
 * Data structure to contain all stats from the UMAC.
 * @warning This is not considered stable API and may change between releases.
 *          For example ordering of fields may change and fields may be removed.
 */
struct mmwlan_stats_umac_data
{
    /** Time that the last TX frame was sent to the driver. */
    uint32_t last_tx_time;

    /** Number of RX frames that have been dropped due to RX Queue being full. */
    uint32_t datapath_rxq_frames_dropped;

    /** Number of TX frames that have been dropped due to TX Queue being full. */
    uint32_t datapath_txq_frames_dropped;

    /** Timestamps taken during the connection process. */
    uint32_t connect_timestamp[MMWLAN_STATS_CONNECT_TIMESTAMP_N_ENTRIES];

    /** Current RSSI value for AP. */
    int16_t rssi;

    /** Count of the number of times the driver has sent a hardware restart notification. */
    uint16_t hw_restart_counter;

    /** Count of the number of successfully conducted scans. */
    uint16_t num_scans_complete;

    /** High water mark of the RX Queue. */
    uint8_t datapath_rxq_high_water_mark;

    /** High water mark of the TX Queue. */
    uint8_t datapath_txq_high_water_mark;

    /** High water mark of the RX Management Queue. */
    uint8_t datapath_rx_mgmt_q_high_water_mark;

    /** Total number of frames that have been dropped due to CCMP failures. (e.g., replay
     *  detection). */
    uint32_t datapath_rx_ccmp_failures;

    /** Number of RX pages that have been dropped due to allocation failures. */
    uint32_t datapath_driver_rx_alloc_failures;

    /** Number of RX pages that have been dropped due to read failures. This includes cases
     *  where pages are dropped due to being malformed. */
    uint32_t datapath_driver_rx_read_failures;

    /** High water mark in terms of number of frames in the RX reorder list. */
    uint8_t datapath_rx_reorder_list_high_water_mark;

    /** Number of frames that have been flushed due to the RX A-MPDU reordering buffer
     *  overflowing. */
    uint32_t datapath_rx_reorder_overflow;

    /** Number of frames that have been flushed from the RX reorder list due to timeout. */
    uint32_t datapath_rx_reorder_timedout;

    /** Number of frames dropped due to outdated sequence control field value. */
    uint32_t datapath_rx_reorder_outdated_drops;

    /** Number of frames dropped due to repeated sequence control field value. */
    uint32_t datapath_rx_reorder_retransmit_drops;

    /** Total number of frames that have actually been put into the A-MPDU reordering buffer. */
    uint32_t datapath_rx_reorder_total;

    /** Number of times any timer or timeout has fired. */
    uint32_t timeouts_fired;
};

/** @} */

/** @} */
