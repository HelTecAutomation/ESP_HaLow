/*
 * Copyright 2021-2024 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @defgroup MMWLAN Morse Micro Wireless LAN (mmwlan) API
 *
 * Wireless LAN control and datapath.
 *
 * @warning Aside from specific exceptions, the functions in this API must not be called
 *          concurrently (e.g., from different thread contexts). The exception to this
 *          is the TX API (@ref mmwlan_tx(), @ref mmwlan_tx_tid(), and @ref mmwlan_tx_pkt()).
 *
 * @section MMWLAN_THREADS Thread priorities
 *
 * The following table documents the threads created by the Morse WLAN driver and Upper MAC
 * included in morselib.
 *
 * | Thread            | Thread Name             | Priority                  |
 * | ----------------- | ----------------------- | ------------------------- |
 * | SPI driver        | @c spi_irq              | @ref MMOSAL_TASK_PRI_HIGH |
 * | Morse driver      | @c drv                  | @ref MMOSAL_TASK_PRI_HIGH |
 * | WLAN event loop   | @c evtloop              | @ref MMOSAL_TASK_PRI_HIGH |
 * | Morse health check| @c health               | @ref MMOSAL_TASK_PRI_LOW  |
 *
 * Note that to get the best performance, the WLAN driver/UMAC threads run at a high priority
 * while it is expected that application threads run at a lower priority.
 *
 * @{
 */

#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "mmpkt.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** Enumeration of status return codes. */
enum mmwlan_status
{
    /** The operation was successful. */
    MMWLAN_SUCCESS,
    /** The operation failed with an unspecified error. */
    MMWLAN_ERROR,
    /** The operation failed due to an invalid argument. */
    MMWLAN_INVALID_ARGUMENT,
    /** Functionality is temporarily unavailable. */
    MMWLAN_UNAVAILABLE,
    /** Unable to proceed because channel list has not been set.
     *  @see mmwlan_set_channel_list(). */
    MMWLAN_CHANNEL_LIST_NOT_SET,
    /** Failed due to memory allocation failure. */
    MMWLAN_NO_MEM,
    /** Failed due to timeout */
    MMWLAN_TIMED_OUT,
    /** Used to indicate that a call to @ref mmwlan_sta_disable() did not shutdown
     *  the transceiver. */
    MMWLAN_SHUTDOWN_BLOCKED,
    /** Attempted to tune to a channel that was not in the regulatory database or not supported. */
    MMWLAN_CHANNEL_INVALID,
    /** The request could not be completed because the given resource was not found. */
    MMWLAN_NOT_FOUND,
    /** Indicates that the operation failed because the UMAC was not running (e.g., the
     *  device was not booted).  */
    MMWLAN_NOT_RUNNING,
    /** Indicates that the operation failed because MMWLAN has not been initialized,
     *  see @ref mmwlan_init() */
    MMWLAN_NOT_INITIALIZED,
    /** Indicates that the specified VIF is not active or that no VIF was specified and a VIF
     *  could not be automatically inferred. */
    MMWLAN_VIF_ERROR,
};

/** Maximum allowable length of an SSID. */
#define MMWLAN_SSID_MAXLEN (32)

/** Maximum allowable length of a passphrase when connecting to an AP. */
#define MMWLAN_PASSPHRASE_MAXLEN (100)

/** Length of a WLAN MAC address. */
#define MMWLAN_MAC_ADDR_LEN (6)

/** Maximum allowable number of EC Groups. */
#define MMWLAN_MAX_EC_GROUPS (4)

/** Size of an 802.11 OUI element in octets. */
#define MMWLAN_OUI_SIZE (3)

/**
 * Enumeration of Virtual Interfaces supported by the MMWLAN API.
 */
enum mmwlan_vif
{
    /** VIF is unspecified. The use of this value depends on the context in which it is used. */
    MMWLAN_VIF_UNSPECIFIED = 0,
    /** STA VIF */
    MMWLAN_VIF_STA = 1,
    /** AP VIF */
    MMWLAN_VIF_AP = 2,
};

/** Enumeration of supported security types. */
typedef enum mmwlan_security_type
{
    /** Open (no security) */
    MMWLAN_OPEN,
    /** Opportunistic Wireless Encryption (OWE) */
    MMWLAN_OWE,
    /** Simultaneous Authentication of Equals (SAE) */
    MMWLAN_SAE,
    MMWLAN_OTHER
}mmwlan_security_type_t;

/** Enumeration of Protected Management Frame (PMF) modes (802.11w). */
enum mmwlan_pmf_mode
{
    /** Protected management frames must be used */
    MMWLAN_PMF_REQUIRED,
    /** No protected management frames */
    MMWLAN_PMF_DISABLED
};

/**
 * @defgroup MMWLAN_REGDB    WLAN Regulatory Database API
 *
 * @{
 *
 * API for configuration of the regulatory domain.
 */

/**
 * If either the global or s1g operating class is set to this, the operating class will
 * not be checked when associating to an AP.
 */
#define MMWLAN_SKIP_OP_CLASS_CHECK -1

/** Regulatory domain information about an S1G channel. */
struct mmwlan_s1g_channel
{
    /** Center frequency of the channel (in Hz). */
    uint32_t centre_freq_hz;
    /** STA Duty Cycle (in 100th of %). */
    uint16_t duty_cycle_sta;
    /** Boolean indicating whether to omit control response frames from duty cycle. */
    bool duty_cycle_omit_ctrl_resp;
    /** Global operating class. If @ref MMWLAN_SKIP_OP_CLASS_CHECK, check is skipped */
    int16_t global_operating_class;
    /** S1G operating class. If @ref MMWLAN_SKIP_OP_CLASS_CHECK, check is skipped */
    int16_t s1g_operating_class;
    /** S1G channel number. */
    uint8_t s1g_chan_num;
    /** Channel operating bandwidth (in MHz). */
    uint8_t bw_mhz;
    /** Maximum transmit power (EIRP in dBm). */
    int8_t max_tx_eirp_dbm;
    /** The length of time to close the tx window between packets (in microseconds). */
    uint32_t pkt_spacing_us;
    /** The minimum packet airtime duration to trigger spacing (in microseconds). */
    uint32_t airtime_min_us;
    /** The maximum allowable packet airtime duration (in microseconds). */
    uint32_t airtime_max_us;
};

/** Length of the two character country code string (null-terminated). */
#define MMWLAN_COUNTRY_CODE_LEN 3

/** A list of S1G channels supported by a given regulatory domain. */
struct mmwlan_s1g_channel_list
{
    /** Two character country code (null-terminated) used to identify the regulatory domain. */
    uint8_t country_code[MMWLAN_COUNTRY_CODE_LEN];
    /** The number of channels in the list. */
    unsigned num_channels;
    /** The channel data. */
    const struct mmwlan_s1g_channel *channels;
};

/**
 * Regulatory database data structure. This is a list of @c mmwlan_s1g_channel_list structs, where
 * each channel list corresponds to a regulatory domain.
 */
struct mmwlan_regulatory_db
{
    /** Number of regulatory domains in the database. */
    unsigned num_domains;

    /** The regulatory domain data */
    const struct mmwlan_s1g_channel_list **domains;
};

/**
 * Look up the given country code in the regulatory database and return the matching channel
 * list if found.
 *
 * @param db            The regulatory database.
 * @param country_code  Country code to look up.
 *
 * @returns the matching channel list if found, else NULL.
 */
static inline const struct mmwlan_s1g_channel_list *mmwlan_lookup_regulatory_domain(
    const struct mmwlan_regulatory_db *db,
    const char *country_code)
{
    unsigned ii;

    if (db == NULL)
    {
        return NULL;
    }

    for (ii = 0; ii < db->num_domains; ii++)
    {
        const struct mmwlan_s1g_channel_list *channel_list = db->domains[ii];
        if (channel_list->country_code[0] == country_code[0] &&
            channel_list->country_code[1] == country_code[1])
        {
            return channel_list;
        }
    }
    return NULL;
}

/**
 * Set the list of channels that are supported by the regulatory domain in which the device
 * resides.
 *
 * @note Must be invoked after WLAN initialization (see @ref mmwlan_init()) but only when inactive
 *       (i.e., STA not enabled).
 *
 * @warning This function takes a reference to the given channel list. It expects the channel
 *          list to remain valid in memory as long as WLAN is in use, or until a new channel
 *          list is configured.
 *
 * @param channel_list  The channel list to set. The list must remain valid in memory.
 *
 * @return @ref MMWLAN_SUCCESS if the channel list was valid and updated successfully,
 *         @ref MMWLAN_UNAVAILABLE if the WLAN subsystem was currently active.
 */
enum mmwlan_status mmwlan_set_channel_list(const struct mmwlan_s1g_channel_list *channel_list);

/** Enumeration of Duty Cycle modes. */
enum mmwlan_duty_cycle_mode
{
    /** Duty cycle air time evenly spread. */
    MMWLAN_DUTY_CYCLE_MODE_SPREAD = 0,
    /** Duty cycle air time available in burst. */
    MMWLAN_DUTY_CYCLE_MODE_BURST = 1,
};

/**
 * Configure the duty cycle behavior for air time distribution.
 *
 * @note This only guaranteed to take effect on subsequent invocation of @ref mmwlan_sta_enable()
 *       or @ref mmwlan_ap_enable().
 *
 * @param  duty_cycle_mode Sets the duty cycle mode. See @ref mmwlan_duty_cycle_mode for what each
 *                         mode means.
 *
 * @return                 @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_set_duty_cycle_mode(enum mmwlan_duty_cycle_mode duty_cycle_mode);

/**
 * Duty cycle configuration and statistics
 */
struct mmwlan_duty_cycle_stats
{
    /** Target duty cycle in 100th of a %, i.e. 1..10000. */
    uint32_t duty_cycle;
    /** Configured duty cycle mode, see @ref mmwlan_duty_cycle_mode */
    enum mmwlan_duty_cycle_mode mode;
    /** Airtime remaining (us) - applicable in burst mode only */
    uint32_t burst_airtime_remaining_us;
    /** Burst window duration (us) - applicable in burst mode only */
    uint32_t burst_window_duration_us;
};

/**
 * Retrieve the transmit duty cycle configuration and statistics.
 *
 * @param stats Pointer to a duty cycle statistics structure.
 *
 * @return      @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_get_duty_cycle_stats(struct mmwlan_duty_cycle_stats *stats);

/** @} */

/**
 * @defgroup MMWLAN_CTRL    WLAN General Control API
 *
 * @{
 *
 * API for general control and configuration that is not linked to a specific operational mode.
 */

/** Maximum length of the Morselib version string. */
#define MMWLAN_MORSELIB_VERSION_MAXLEN (32)
/** Maximum length of the firmware version string. */
#define MMWLAN_FW_VERSION_MAXLEN (32)
/** Maximum length of the chip id string. */
#define MMWLAN_CHIP_ID_STRING_MAXLEN (32)

/** Structure for retrieving version information from the mmwlan subsystem. */
struct mmwlan_version
{
    /** Morselib version string. Null terminated. */
    char morselib_version[MMWLAN_MORSELIB_VERSION_MAXLEN];
    /** Morse transceiver firmware version string. Null terminated. */
    char morse_fw_version[MMWLAN_FW_VERSION_MAXLEN];
    /** Morse transceiver chip ID. */
    uint32_t morse_chip_id;
    /** Morse transceiver chip ID user-friendly string. */
    char morse_chip_id_string[MMWLAN_CHIP_ID_STRING_MAXLEN];
};

/**
 * Retrieve version information from morselib and the connected Morse transceiver.
 *
 * @param version   The data structure to fill out with version information.
 *
 * @note If the Morse transceiver has not previously been powered on then the @c fw_version
 *       field of @p version will be set to a zero length string.
 *
 * @returns @c MMWLAN_SUCCESS on success else an error code.
 */
enum mmwlan_status mmwlan_get_version(struct mmwlan_version *version);

/** Maximum length of a BCF board description string (excluding null terminator). */
#define MMWLAN_BCF_BOARD_DESC_MAXLEN (31)
/** Maximum length of a BCF build version string (excluding null terminator). */
#define MMWLAN_BCF_BUILD_VERSION_MAXLEN (31)

/** Board configuration file (BCF) metadata. */
struct mmwlan_bcf_metadata
{
    /** BCF semantic version. */
    struct
    {
        /** Major version field. */
        uint16_t major;
        /** Minor version field. */
        uint8_t minor;
        /** Patch version field. */
        uint8_t patch;
    } version;

    /**
     * Board description string. This is a free form text field included in the BCF.
     *
     * This string will be null-terminated and if it exceeds @ref MMWLAN_BCF_BOARD_DESC_MAXLEN
     * characters in length it will be truncated.
     */
    char board_desc[MMWLAN_BCF_BOARD_DESC_MAXLEN + 1];

    /**
     * Build version string.
     *
     * This string will be null-terminated and if it exceeds @ref MMWLAN_BCF_BOARD_DESC_MAXLEN
     * characters in length it will be truncated.
     */
    char build_version[MMWLAN_BCF_BUILD_VERSION_MAXLEN + 1];
};

/**
 * Read the metadata from the board configuration file (BCF).
 *
 * @param metadata  Pointer to a metadata data structure to be filled out on success.
 *
 * @returns @c MMWLAN_SUCCESS on success else an error code.
 */
enum mmwlan_status mmwlan_get_bcf_metadata(struct mmwlan_bcf_metadata *metadata);

/**
 * Override the maximum TX power. If no override is specified then the maximum TX power used
 * will be the maximum TX power allowed for the channel in the current regulatory domain.
 *
 * @note Must be invoked after WLAN initialization (see @ref mmwlan_init()). Only takes
 *       effect when switching channel (e.g., during scan or AP connection procedure).
 *
 * @note This will not increase the TX power over the maximum allowed for the current channel
 *       in the configured regulatory domain. Therefore, this override in effect will only
 *       reduce the maximum TX power and cannot increase it.
 *
 * @param tx_power_dbm  The maximum TX power override to set (in dBm). Set to zero to disable
 *                      the override.
 *
 * @return @ref MMWLAN_SUCCESS if the country code was valid and updated successfully,
 *         @ref MMWLAN_UNAVAILABLE if the WLAN subsystem was currently active.
 */
enum mmwlan_status mmwlan_override_max_tx_power(uint16_t tx_power_dbm);

/**
 * Set the RTS threshold.
 *
 * When packets larger than the RTS threshold are transmitted they are protected by an RTS/CTS
 * exchange.
 *
 * @param rts_threshold The RTS threshold (in octets) to set, or 0 to disable.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_set_rts_threshold(unsigned rts_threshold);

/**
 * Sets whether or not Short Guard Interval (SGI) support is enabled. Defaults to enabled
 * if not set otherwise.
 *
 * @note This will not force use of SGI, only enable support for it. The rate control algorithm
 *       will make the decision as to which guard interval to use unless explicitly overridden
 *       by @ref mmwlan_ate_override_rate_control().
 *
 * @note This must only be invoked when MMWLAN is inactive (i.e., STA mode not enabled).
 *
 * @param sgi_enabled   Boolean value indicating whether SGI support should be enabled.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_set_sgi_enabled(bool sgi_enabled);

/**
 * Sets whether or not sub-band support is enabled for transmit. Defaults to enabled
 * if not set otherwise.
 *
 * @note This will not force use of sub-bands, only enable support for it. The rate control
 *       algorithm will make the decision as to which bandwidth to use unless explicitly overridden
 *       by @ref mmwlan_ate_override_rate_control().
 *
 * @note This must only be invoked when MMWLAN is inactive (i.e., STA mode not enabled).
 *
 * @param subbands_enabled   Boolean value indicating whether sub-band support should be enabled.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_set_subbands_enabled(bool subbands_enabled);

/**
 * Sets whether or not Aggregated MAC Protocol Data Unit (A-MPDU) support is enabled.
 * This defaults to enabled, if not set otherwise.
 *
 * @note This must only be invoked when MMWLAN is inactive (i.e., STA mode not enabled).
 *
 * @param ampdu_enabled   Boolean value indicating whether AMPDU support should be enabled.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_set_ampdu_enabled(bool ampdu_enabled);

/**
 * Minimum value of fragmentation threshold that can be set with
 * @ref mmwlan_set_fragment_threshold().
 */
#define MMWLAN_MINIMUM_FRAGMENT_THRESHOLD (256)

/**
 * Set the Fragmentation threshold.
 *
 * Maximum length of the frame, beyond which packets must be fragmented into two or more frames.
 *
 * @note Even if the fragmentation threshold is set to 0 (disabled), fragmentation may still occur
 *       if a given packet is too large to be transmitted at the selected rate.
 *
 * @warning Setting a fragmentation threshold may have unintended side effects due to restrictions
 *          at lower bandwidths and MCS rates. In normal operation the fragmentation threshold
 *          should be disabled, in which case packets will be fragmented automatically as necessary
 *          based on the selected rate.
 *
 * @param fragment_threshold The fragmentation threshold (in octets) to set,
 *                           or zero to disable the fragmentation threshold. Minimum value
 *                           (if not zero) is given by @ref MMWLAN_MINIMUM_FRAGMENT_THRESHOLD.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_set_fragment_threshold(unsigned fragment_threshold);

/** The total number of @ref mmwlan_qos_queue_params that exist. */
#define MMWLAN_QOS_QUEUE_NUM_ACIS 4

/** Structure for storing QoS queue parameters  */
struct mmwlan_qos_queue_params
{
    /** Access Category Index [0..3]. */
    uint8_t aci;
    /** Arbitration Inter-frame Space [2..255] */
    uint8_t aifs;
    /** Minimum Contention Window */
    uint16_t cw_min;
    /** Maximum Contention Window */
    uint16_t cw_max;
    /** Maximum burst time in microseconds, 0 meaning disabled. */
    uint32_t txop_max_us;
};

/**
 * Updates the default QoS queue configuration to the given values.
 * These values will be made active while the station is connecting to an Access Point.
 *
 * @note Although the active configuration will be changed to the Access Point's configurations
 *       for these values after connecting, these default values will not be overwritten and will
 *       be reactivated after the station disconnects.
 *
 * @param params Array of QoS queue parameters. This array does not need to be sorted in Access
 *               Category Index (ACI) order, since the ACI is specified as part of the
 *               @c mmwlan_qos_queue_params structure. The same ACI must not be specified more
 *               than once in this array. If a parameter for a given ACI is not included in this
 *               list then its configuration will be left unchanged.
 *
 * @param count  The number of elements in the @c params array.
 *               Must be at least 1 and no more than @ref MMWLAN_QOS_QUEUE_NUM_ACIS.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_set_default_qos_queue_params(const struct mmwlan_qos_queue_params *params,
                                                       size_t count);

/** Enumeration of configuration states for MCS10 behavior. */
enum mmwlan_mcs10_mode
{
    /** MCS10 is disabled. */
    MMWLAN_MCS10_MODE_DISABLED = 0x00,
    /** Always use MCS10 instead of MCS 0 if the bandwidth is 1 MHz. */
    MMWLAN_MCS10_MODE_FORCED = 0x01,
    /** Use MCS10 on retries instead of MCS 0 if the bandwidth is 1 MHz. */
    MMWLAN_MCS10_MODE_AUTO = 0x02
};

/**
 * Configure the rate adaptation behavior around selecting MCS10.
 *
 * @param mcs10_mode   Sets the MCS10 mode. See @ref mmwlan_mcs10_mode for what each mode means.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_set_mcs10_mode(enum mmwlan_mcs10_mode mcs10_mode);

/**
 * Enables the 1MHz control response override. This means that in response to directed packets,
 * the control responses (e.g. an NDP ACK or Block ACK) will be sent at 1MHz.
 *
 * @note Must be invoked after WLAN initialization (see @ref mmwlan_init()) but only when inactive
 *       (i.e., STA not enabled).
 *
 * @param enabled Whether to send control response preambles in 1MHz bandwidth. True to transmit in
 *                1MHz. False to disable this override.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_set_control_response_preamble_1mhz_out_en(bool enabled);

/**
 * The default minimum interval to wait after the last health check before triggering another.
 */
#ifndef MMWLAN_DEFAULT_MIN_HEALTH_CHECK_INTERVAL_MS
#define MMWLAN_DEFAULT_MIN_HEALTH_CHECK_INTERVAL_MS 60000
#endif

/**
 * The default maximum interval to wait after the last health check before triggering another.
 */
#ifndef MMWLAN_DEFAULT_MAX_HEALTH_CHECK_INTERVAL_MS
#define MMWLAN_DEFAULT_MAX_HEALTH_CHECK_INTERVAL_MS 120000
#endif

/**
 * Specify the upper and lower bound for the periodic health check interval. To guarantee a specific
 * interval set both @c min_interval_ms and @c max_interval_ms to the same value.
 *
 * @note To disable periodic health checks entirely set both values to zero (0).
 *
 * @param min_interval_ms Minimum value that the interval can be.
 * @param max_interval_ms Maximum value that the interval can be.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_set_health_check_interval(uint32_t min_interval_ms,
                                                    uint32_t max_interval_ms);

/**
 * Arguments data structure for @ref mmwlan_boot().
 *
 * This structure should be initialized using @ref MMWLAN_BOOT_ARGS_INIT for sensible
 * default values, particularly for forward compatibility with new releases that may add
 * new fields to the struct. For example:
 *
 * @note This struct currently does not include any sensible arguments yet, and is provided
 *       for forwards compatibility with potential future API extensions.
 *
 * @code{.c}
 *     enum mmwlan_status status;
 *     struct mmwlan_boot_args boot_args = MMWLAN_BOOT_ARGS_INIT;
 *     // HERE: initialize arguments
 *     status = mmwlan_boot(&boot_args);
 * @endcode
 */
struct mmwlan_boot_args
{
    /** Note this field should not be used and will be removed in future. */
    uint8_t reserved;
};
extern enum mmwlan_vif halow_mode;
/**
 * Initializer for @ref mmwlan_boot_args.
 *
 * @see mmwlan_boot_args
 */
#define MMWLAN_BOOT_ARGS_INIT { 0 }

/**
 * Boot the Morse Micro transceiver and leave it in an idle state.
 *
 * @note In general, it is not necessary to use this function as @ref mmwlan_sta_enable()
 *       will automatically boot the chip if required. It may be used, for example, to power
 *       on the chip for production test, etc.
 *
 * @warning Channel list must be set before booting the transceiver. @ref mmwlan_set_channel_list().
 *
 * @param args  Boot arguments. May be @c NULL, in which case default values will be used.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_boot(const struct mmwlan_boot_args *args);

/**
 * Perform a clean shutdown of the Morse Micro transceiver, including cleanly disconnecting
 * from a connected AP, if necessary.
 *
 * Has no effect if the transceiver is already shutdown.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_shutdown(void);

/**
 * Gets the MAC address of the given interface.
 *
 * The STA MAC address (vif = MMWLAN_VIF_STA) comes from one of the following sources,
 * in descending priority order:
 *
 * 1. @c mmhal_read_mac_addr()
 * 2. Morse Micro transceiver
 * 3. Randomly generated in the form `02:01:XX:XX:XX:XX`
 *
 * @note If a MAC address override is not provided (via @c mmhal_read_mac_addr()) then the
 *       transceiver must have been booted at least once before this function is invoked.
 *
 * The AP MAC address (vif = MMWLAN_VIF_AP) is equivalent to the BSSID.
 *
 * @param vif       The VIF to get the MAC address of.
 * @param mac_addr  Buffer to receive the MAC address. Length must be @ref MMWLAN_MAC_ADDR_LEN.
 *
 * @return @ref MMWLAN_SUCCESS on success, @ref MMWLAN_UNAVAILABLE if the MAC address was not
 *         able to be read from the transceiver because it was not booted, else an appropriate
 *         error code.
 */
enum mmwlan_status mmwlan_get_vif_mac_addr(enum mmwlan_vif vif, uint8_t *mac_addr);

/**
 * Gets the MAC address of the STA interface.
 *
 * @deprecated This function is deprecated and provided for backwards compatibility.
 *             @ref mmwlan_get_vif_mac_addr should be used for new developments.
 *
 * This function is equivalent to @c mmwlan_get_vif_mac_addr(MMWLAN_VIF_STA, mac_addr)
 *
 * @param mac_addr  Buffer to receive the MAC address. Length must be @ref MMWLAN_MAC_ADDR_LEN.
 *
 * @return @ref MMWLAN_SUCCESS on success, @ref MMWLAN_UNAVAILABLE if the MAC address was not
 *         able to be read from the transceiver because it was not booted, else an appropriate
 *         error code.
 */
static inline enum mmwlan_status mmwlan_get_mac_addr(uint8_t *mac_addr)
{
    return mmwlan_get_vif_mac_addr(halow_mode, mac_addr);
}

/** @} */

/**
 * @defgroup MMWLAN_SCAN    WLAN Control API for Scan
 *
 * @{
 *
 * API for performing WLAN Scan operations.
 */

/** Result of the scan request. */
struct mmwlan_scan_result
{
    /** RSSI of the received frame. */
    int16_t rssi;
    /** Pointer to the BSSID field within the Probe Response frame. */
    const uint8_t *bssid;
    /** Pointer to the SSID within the SSID IE of the Probe Response frame. */
    const uint8_t *ssid;
    /** Pointer to the start of the Information Elements within the Probe Response frame. */
    const uint8_t *ies;
    /** Value of the Beacon Interval field. */
    uint16_t beacon_interval;
    /** Value of the Capability Information field. */
    uint16_t capability_info;
    /** Length of the Information Elements (@c ies). */
    uint16_t ies_len;
    /** Length of the SSID (@c ssid). */
    uint8_t ssid_len;
    /** Center frequency in Hz of the channel where the frame was received. */
    uint32_t channel_freq_hz;
    /** Bandwidth, in MHz, where the frame was received. */
    uint8_t bw_mhz;
    /** Operating bandwidth, in MHz, of the access point. */
    uint8_t op_bw_mhz;
    /**
     * Background noise measured by the chip on the channel at the time
     * the probe response was received.
     */
    int8_t noise_dbm;
    /** TSF timestamp in the Probe Response frame. */
    uint64_t tsf;
};

/** mmwlan scan rx callback function prototype. */
typedef void (*mmwlan_scan_rx_cb_t)(const struct mmwlan_scan_result *result, void *arg);

/**
 * Scan configuration data structure.
 *
 * Use @ref MMWLAN_SCAN_CONFIG_INIT for initialization. For example:
 *
 * @code{.c}
 * struct mmwlan_scan_config scan_config = MMWLAN_SCAN_CONFIG_INIT;
 * @endcode
 *
 * @see mmwlan_set_scan_config()
 */
struct mmwlan_scan_config
{
    /**
     * Set the per-channel dwell time to use for scans that are requested internally within the
     * mmwlan driver (e.g., when connecting or background scanning).
     *
     * @note This does not affect scans requested with the @ref mmwlan_scan_request().
     */
    uint32_t dwell_time_ms;

    /**
     * Boolean value indicating whether NDP probe support should be enabled.
     *
     * NDP probe requests are smaller than regular probe requests and will save energy when
     * scanning.
     *
     * @warning Be careful when enabling NDP probe requests. Some APs may not respond to NDP probe
     *          requests if the CSSID (Compressed SSID) field is not populated. When using the
     *          @ref mmwlan_scan_request() API with NDP probe requests enabled, it is advisable to
     *          include an SSID in the scan arguments (see @ref mmwlan_scan_args.ssid).
     */
    bool ndp_probe_enabled;

    /**
     * Set the home channel dwell time to use for scans that are requested internally within the
     * mmwlan driver (e.g., when connecting or background scanning).
     *
     * @note This does not affect scans requested with the @ref mmwlan_scan_request().
     */
    uint32_t home_channel_dwell_time_ms;
};

/** Initializer for @ref mmwlan_scan_config. */
#define MMWLAN_SCAN_CONFIG_INIT \
    { MMWLAN_SCAN_DEFAULT_DWELL_TIME_MS, false, MMWLAN_SCAN_DEFAULT_DWELL_ON_HOME_MS }

/**
 * Update the scan configuration with the given settings.
 *
 * @param config    The new configuration to set.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_set_scan_config(const struct mmwlan_scan_config *config);
/** Default value for @c mmwlan_scan_args.dwell_time_ms. Note that reducing the dwell time
 *  below this value may impact scan reliability. */
#define MMWLAN_SCAN_DEFAULT_DWELL_TIME_MS (30)
/** Default time to dwell on home channel, in between scan channels */
#define MMWLAN_SCAN_DEFAULT_DWELL_ON_HOME_MS (200)

/** Minimum value for @c mmwlan_scan_args.dwell_time_ms. */
#define MMWLAN_SCAN_MIN_DWELL_TIME_MS (15)

/**
 * Enumeration of states in Scan mode.
 */
enum mmwlan_scan_state
{
    /** Scan was successful and all channels were scanned. */
    MMWLAN_SCAN_SUCCESSFUL,
    /** Scan was incomplete. One or more channels may have been scanned and therefore an
     *  incomplete set of scan results may still have been received. */
    MMWLAN_SCAN_TERMINATED,
    /** Scanning in progress. */
    MMWLAN_SCAN_RUNNING,
};

/** mmwlan scan complete callback function prototype. */
typedef void (*mmwlan_scan_complete_cb_t)(enum mmwlan_scan_state scan_state, void *arg);

/**
 * Structure to hold scan arguments. This structure should be initialized using
 * @ref MMWLAN_SCAN_ARGS_INIT for forward compatibility.
 */
struct mmwlan_scan_args
{
    /**
     * Minimum time to dwell on a channel waiting for probe responses/beacons.
     *
     * @note There is some additional delay applied on top of this to allow for tuning to each
     *       channel and sending a probe request.
     */
    uint32_t dwell_time_ms;
    /**
     * Extra Information Elements to include in Probe Request frames.
     * May be @c NULL if @c extra_ies_len is zero.
     */
    uint8_t *extra_ies;
    /** Length of @c extra_ies. */
    size_t extra_ies_len;
    /**
     * SSID used for scan.
     * May be @c NULL, with @c ssid_len set to zero for an undirected scan.
     */
    uint8_t ssid[MMWLAN_SSID_MAXLEN];
    /** Length of the SSID. */
    uint16_t ssid_len;
    /**
     * Time to dwell on home channel in between channels during a scan, to allow traffic
     * to still pass. This will only perform while connected to an AP and is ignored otherwise.
     * If set to 0, the device will not return to the home channel during the scan.
     */
    uint32_t dwell_on_home_ms;
};

/**
 * Initializer for @ref mmwlan_scan_args.
 *
 * For example:
 *
 * @code{c}
 * struct mmwlan_scan_args scan_args = MMWLAN_SCAN_ARGS_INIT;
 * @endcode
 */
#define MMWLAN_SCAN_ARGS_INIT                                     \
    {                                                             \
        .dwell_time_ms = MMWLAN_SCAN_DEFAULT_DWELL_TIME_MS,       \
        .extra_ies = NULL,                                        \
        .extra_ies_len = 0,                                       \
        .ssid = { 0 },                                            \
        .ssid_len = 0,                                            \
        .dwell_on_home_ms = MMWLAN_SCAN_DEFAULT_DWELL_ON_HOME_MS, \
    }

/**
 * Structure to hold arguments specific to a given instance of a scan.
 */
struct mmwlan_scan_req
{
    /** Scan response receive callback. Must not be @c NULL. */
    mmwlan_scan_rx_cb_t scan_rx_cb;
    /** Scan complete callback. Must not be @c NULL. */
    mmwlan_scan_complete_cb_t scan_complete_cb;
    /** Opaque argument to be passed to the callbacks. */
    void *scan_cb_arg;
    /** Scan arguments to be used @ref mmwlan_scan_args. */
    struct mmwlan_scan_args args;
};

/**
 * Initializer for @ref mmwlan_scan_req.
 *
 * For example:
 *
 * @code{c}
 * struct mmwlan_scan_req scan_req = MMWLAN_SCAN_REQ_INIT;
 * @endcode
 */
#define MMWLAN_SCAN_REQ_INIT { NULL, NULL, NULL, MMWLAN_SCAN_ARGS_INIT }

/**
 * Request a scan.
 *
 * If the transceiver is not already powered on, it will be powered on before the scan is
 * initiated. The power on procedure will block this function. The transceiver will remain
 * powered on after scan completion and must be shutdown by invoking @ref mmwlan_shutdown().
 *
 * @note Just because a scan is requested does not mean it will happen immediately.
 *       It may take some time for the request to be serviced.
 *
 * @note The scan request may be rejected, in which case the complete callback will be
 *       invoked immediately with an error status.
 *
 * @param scan_req          Scan request instance. See @ref mmwlan_scan_req.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_scan_request(const struct mmwlan_scan_req *scan_req);

/**
 * Abort in progress or pending scans.
 *
 * The scan callback will be called back with result code @ref MMWLAN_SCAN_TERMINATED for
 * all aborted scans.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_scan_abort(void);

/** @} */

/**
 * @defgroup MMWLAN_STA WLAN Control API for Station (STA) mode
 *
 * @{
 *
 * API for configuration and control of Station (STA) mode.
 */

/** Default Background scan short interval in seconds.
 *  Setting to 0 will disable background scanning. */
#define DEFAULT_BGSCAN_SHORT_INTERVAL_S (0)

/** Default Background scan signal threshold in dBm. */
#define DEFAULT_BGSCAN_THRESHOLD_DBM (0)

/** Default Background scan long interval in seconds.
 *  Setting to 0 will disable background scanning. */
#define DEFAULT_BGSCAN_LONG_INTERVAL_S (0)

/** Default Target Wake Time (TWT) interval in micro seconds. */
#define DEFAULT_TWT_WAKE_INTERVAL_US (300000000)

/** Default min Target Wake Time (TWT) duration in micro seconds. */
#define DEFAULT_TWT_MIN_WAKE_DURATION_US (65280)

/** Default value for the @c scan_interval_base_s field of @ref mmwlan_sta_args. */
#define MMWLAN_DEFAULT_SCAN_INTERVAL_BASE_S (2)

/** Default value for the @c scan_interval_limit_s field of @ref mmwlan_sta_args. */
#define MMWLAN_DEFAULT_SCAN_INTERVAL_LIMIT_S (512)

/** Maximum allowable Restricted Access Window (RAW) priority for STA. */
#define MMWLAN_RAW_MAX_PRIORITY (7)

/** Enumeration of Centralized Authentication Control (CAC) modes. */
enum mmwlan_cac_mode
{
    /** CAC disabled */
    MMWLAN_CAC_DISABLED,
    /** CAC enabled */
    MMWLAN_CAC_ENABLED
};

/** Enumeration of Linux 4-address mode settings. */
enum mmwlan_4addr_mode
{
    /** 4 Address Mode disabled */
    MMWLAN_4ADDR_MODE_DISABLED,
    /** 4 Address Mode enabled */
    MMWLAN_4ADDR_MODE_ENABLED,
};

/** Enumeration of supported 802.11 power save modes. */
enum mmwlan_ps_mode
{
    /** Power save disabled */
    MMWLAN_PS_DISABLED,
    /** Power save enabled */
    MMWLAN_PS_ENABLED
};

/**
 * Enumeration of S1G non-AP STA types.
 */
enum mmwlan_station_type
{
    MMWLAN_STA_TYPE_SENSOR = 0x01,
    MMWLAN_STA_TYPE_NON_SENSOR = 0x02,
};

/**
 * Enumeration of states in STA mode.
 */
enum mmwlan_sta_state
{
    MMWLAN_STA_DISABLED,
    MMWLAN_STA_CONNECTING,
    MMWLAN_STA_CONNECTED,
};

/** STA status callback function prototype. */
typedef void (*mmwlan_sta_status_cb_t)(enum mmwlan_sta_state sta_state);

/**
 * Enumeration of STA events.
 *
 * @warning BETA NOTICE: This is beta API that is under development;
 *          breaking changes may be introduced in future releases.
 */
enum mmwlan_sta_event
{
    /** The STA is starting a scan. */
    MMWLAN_STA_EVT_SCAN_REQUEST,
    /** The STA has finished a scan. */
    MMWLAN_STA_EVT_SCAN_COMPLETE,
    /** The STA has aborted a scan early. */
    MMWLAN_STA_EVT_SCAN_ABORT,
    /** The STA is sending an authentication request to the AP. */
    MMWLAN_STA_EVT_AUTH_REQUEST,
    /** The STA is sending an association request to the AP. */
    MMWLAN_STA_EVT_ASSOC_REQUEST,
    /** The STA is sending a de-authorization request to the AP. */
    MMWLAN_STA_EVT_DEAUTH_TX,
    /**
     * The Supplicant IEEE 802.1X Controlled Port is now open meaning that
     * the STA is fully authenticated and data transmission can begin.
     * */
    MMWLAN_STA_EVT_CTRL_PORT_OPEN,
    /**  The Supplicant IEEE 802.1X Controlled Port is now closed. */
    MMWLAN_STA_EVT_CTRL_PORT_CLOSED,
};

/** Argument passed to the STA event callback. */
struct mmwlan_sta_event_cb_args
{
    /** The event that triggered the callback. */
    enum mmwlan_sta_event event;
};

/**
 * STA event callback prototype.
 *
 * @warning BETA NOTICE: This is beta API that is under development;
 *          breaking changes may be introduced in future releases.
 */
typedef void (*mmwlan_sta_event_cb_t)(const struct mmwlan_sta_event_cb_args *sta_event, void *arg);

/**
 * Arguments data structure for @ref mmwlan_sta_enable().
 *
 * This structure should be initialized using @ref MMWLAN_STA_ARGS_INIT for sensible
 * default values, particularly for forward compatibility with new releases that may add
 * new fields to the struct. For example:
 *
 * @code{.c}
 *     enum mmwlan_status status;
 *     struct mmwlan_sta_args sta_args = MMWLAN_STA_ARGS_INIT;
 *     // HERE: initialize arguments
 *     status = mmwlan_sta_enable(&sta_args);
 * @endcode
 */
struct mmwlan_sta_args
{
    /** SSID of the AP to connect to. */
    uint8_t ssid[MMWLAN_SSID_MAXLEN];
    /** Length of the SSID. */
    uint16_t ssid_len;
    /**
     * BSSID of the AP to connect to.
     * If non-zero, the STA will only connect to an AP that matches this value.
     */
    uint8_t bssid[MMWLAN_MAC_ADDR_LEN];
    /** Type of security to use. If @c MMWLAN_SAE then a @c passphrase must be specified. */
    enum mmwlan_security_type security_type;
    /** Passphrase (only used if @c security_type is @c MMWLAN_SAE, otherwise ignored. */
    char passphrase[MMWLAN_PASSPHRASE_MAXLEN + 1];
    /** Length of @c passphrase. May be zero if @c passphrase is null-terminated. */
    uint16_t passphrase_len;
    /** Protected Management Frame mode to use (802.11w) */
    enum mmwlan_pmf_mode pmf_mode;
    /**
     * Priority used by the AP to assign a STA to a Restricted Access Window (RAW) group.
     * Valid range is 0 - @ref MMWLAN_RAW_MAX_PRIORITY, or -1 to disable RAW.
     */
    int16_t raw_sta_priority;
    /** S1G non-AP STA type. For valid STA types, @ref mmwlan_station_type */
    enum mmwlan_station_type sta_type;
    /**
     * Preference list of enabled elliptic curve groups for SAE and OWE.
     * By default (if this parameter is not set), the mandatory group 19 is preferred.
     */
    int sae_owe_ec_groups[MMWLAN_MAX_EC_GROUPS];
    /** Whether Centralized Authentication Controlled is enabled on the STA. */
    enum mmwlan_cac_mode cac_mode;
    /**
     * Background scan short interval, measured in seconds.
     *
     * When the signal strength falls below @c bgscan_signal_threshold_dbm, this interval
     * will be used between iterations of background scan. After several iterations, or
     * if the signal threshold increases above @c bgscan_signal_threshold_dbm background
     * scan will return to using the long interval.
     *
     * @note Setting this to zero will disable background scanning.
     */
    uint16_t bgscan_short_interval_s;
    /**
     * Background scan signal strength threshold that switches between short and long intervals.
     */
    int bgscan_signal_threshold_dbm;
    /**
     * Background scan long interval, measured in seconds.
     *
     * When the signal strength is above @c bgscan_signal_threshold_dbm, this interval
     * will be used between iterations of background scan. If the signal threshold
     * falls below @c bgscan_signal_threshold_dbm, background scan will use the short
     * interval.
     *
     * @note Setting this to zero will disable background scanning.
     */
    uint16_t bgscan_long_interval_s;
    /** Optional callback for scan results which are received during the connection process. */
    mmwlan_scan_rx_cb_t scan_rx_cb;
    /** Opaque argument to be passed to @ref scan_rx_cb. */
    void *scan_rx_cb_arg;
    /**
     * The base scan interval (in seconds) to use when (re)connecting. An exponential back off
     * is applied such that if the AP is not found during the first scan, we will wait for
     * @c scan_interval_base_s seconds before attempting the second scan, then
     * @c scan_interval_base_s squared seconds before attempting for the next scan, and so
     * on until @c scan_interval_limit_s is reached.
     *
     * If this is 0 then the @ref MMWLAN_DEFAULT_SCAN_INTERVAL_BASE_S will be used.
     */
    uint16_t scan_interval_base_s;
    /**
     * The maximum interval between scan attempts when (re)connecting. The scan algorithm will
     * begin with an interval of @c scan_interval_base_s between scans and increase the interval
     * exponentially until this limit is reached.
     *
     * If this is 0 then the @ref MMWLAN_DEFAULT_SCAN_INTERVAL_LIMIT_S will be used.
     */
    uint16_t scan_interval_limit_s;
    /**
     * Extra Information Elements to include in association request frames.
     * Should be @c NULL if @c extra_assoc_ies_len is zero.
     * It is the caller's responsibility to handle potentially duplicate IEs.
     * It is the caller's responsibility to free this buffer after calling @ref mmwlan_sta_enable().
     */
    uint8_t *extra_assoc_ies;
    /** Length of @c extra_assoc_ies */
    size_t extra_assoc_ies_len;
    /**
     * STA event callback with a user-defined opaque parameter. May be @c NULL.
     *
     * @warning BETA NOTICE: This is beta API that is under development;
     *          breaking changes may be introduced in future releases.
     */
    mmwlan_sta_event_cb_t sta_evt_cb;
    /**
     * STA event callback argument to be passed to @c sta_evt_cb. May optionally be @c NULL.
     * The value of this parameter must remain valid during the lifetime of the connection.
     *
     * @warning BETA NOTICE: This is beta API that is under development;
     *          breaking changes may be introduced in future releases.
     */
    void *sta_evt_cb_arg;
    /**
     * Whether the station should use Linux 4-address mode. When connected to a Linux AP in
     * 4-address mode, this will trigger the AP to move the STA to a separate virtual interface.
     */
    enum mmwlan_4addr_mode use_4addr;
};

/**
 * Initializer for @ref mmwlan_sta_args.
 *
 * @see mmwlan_sta_args
 */
#define MMWLAN_STA_ARGS_INIT                                           \
    {                                                                  \
        .ssid = { 0 },                                                 \
        .ssid_len = 0,                                                 \
        .bssid = { 0 },                                                \
        .security_type = MMWLAN_OPEN,                                  \
        .passphrase = { 0 },                                           \
        .passphrase_len = 0,                                           \
        .pmf_mode = MMWLAN_PMF_REQUIRED,                               \
        .raw_sta_priority = -1,                                        \
        .sta_type = MMWLAN_STA_TYPE_NON_SENSOR,                        \
        .sae_owe_ec_groups = { 0 },                                    \
        .cac_mode = MMWLAN_CAC_DISABLED,                               \
        .bgscan_short_interval_s = DEFAULT_BGSCAN_SHORT_INTERVAL_S,    \
        .bgscan_signal_threshold_dbm = DEFAULT_BGSCAN_THRESHOLD_DBM,   \
        .bgscan_long_interval_s = DEFAULT_BGSCAN_LONG_INTERVAL_S,      \
        .scan_rx_cb = NULL,                                            \
        .scan_rx_cb_arg = NULL,                                        \
        .scan_interval_base_s = MMWLAN_DEFAULT_SCAN_INTERVAL_BASE_S,   \
        .scan_interval_limit_s = MMWLAN_DEFAULT_SCAN_INTERVAL_LIMIT_S, \
        .extra_assoc_ies = NULL,                                       \
        .extra_assoc_ies_len = 0,                                      \
        .sta_evt_cb = NULL,                                            \
        .sta_evt_cb_arg = NULL,                                        \
        .use_4addr = MMWLAN_4ADDR_MODE_DISABLED,                       \
    }

/**
 * Enable station mode.
 *
 * This will power on the transceiver then initiate connection to the given Access Point. If
 * station mode is already enabled when this function is invoked then it will disconnect from
 * (if already connected) and initiate connection to the given AP.
 *
 * @note The STA status callback (@p sta_status_cb) must not block and MMWLAN API functions
 *       may not be invoked from the callback.
 * @note A copy of @p args->extra_assoc_ies buffer will be made if @p args->extra_assoc_ies_len is
 *       non-zero. Caller is responsible for freeing the buffer in @p args after this function is
 * called.
 *
 * @warning Channel list must be set before enabling station mode. @ref mmwlan_set_channel_list().
 *
 * @deprecated  The @c sta_status_cb parameter should not be used. Please use
 *              @c mmwlan_sta_args.ext_sta_status_cb as the preferred callback method.
 *
 * @param args              STA arguments (e.g., SSID, etc.). See @ref mmwlan_sta_args.
 * @param sta_status_cb     Optional callback to be invoked on STA state changes.
 *                          May be @c NULL. Must be @c NULL if @c args->ext_sta_status_cb is not @c
 *                          NULL.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_sta_enable(const struct mmwlan_sta_args *args,
                                     mmwlan_sta_status_cb_t sta_status_cb);

/**
 * Disable station mode.
 *
 * This will disconnect from the AP. It will also shut down the transceiver if nothing else
 * is holding it open. Note that if the transceiver was booted by @c mmwlan_boot() then
 * this function will not shut down the transceiver.
 *
 * @return @ref MMWLAN_SUCCESS if successful, else an appropriate error code.
 */
enum mmwlan_status mmwlan_sta_disable(void);

/**
 * Gets the current WLAN STA state.
 *
 * @return the current WLAN STA state.
 */
enum mmwlan_sta_state mmwlan_get_sta_state(void);

/**
 * Sets whether or not the 802.11 power save is enabled. Defaults to @ref MMWLAN_PS_ENABLED
 *
 * @note It is recommended to keep power save enabled if the STA will be duty cycle limited,
 *       @see mmregdb.c for regulatory duty cycle limits.
 *
 * @param mode   enum indicating which 802.11 power save mode to use.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_set_power_save_mode(enum mmwlan_ps_mode mode);

/** Default timeout after network activity to signal sleep */
#define MMWLAN_DEFAULT_DYNAMIC_PS_TIMEOUT_MS 100;

/**
 * Sets the time after network activity before the STA will notify the AP that it will go to sleep
 * using a QoS Null frame and when the host will release its veto (via the wake pin) on chip sleep.
 *
 *                          network traffic                          QoS Null
 *             |----------------------------------------|               |
 *             +--------------------------------------------------------+
 *   Wake Pin  |                                                        |
 *  -----------+                                                        +---------
 *                                                      |---------------|
 *                                                    dynamic_ps_timeout_ms
 *                                                                      |---------
 *                                                                         sleep
 *                                                                       permitted
 *
 * @warning Reducing this value will cause the MM-Chip to sleep more aggressively. This may lead to
 *          unexpected behavior such as increased latency and/or dropped packets.
 *
 * @param timeout_ms Timeout after network activity before signaling sleep.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_set_dynamic_ps_timeout(uint32_t timeout_ms);

/**
 * Sets whether or not non-TIM mode support is enabled.
 * Upon successful non-TIM mode negotiation, the STA will ignore traffic indication map (TIM)
 * and send PS-Poll every listen interval.
 * This defaults to disabled and is for STA mode only.
 *
 * @note This must only be invoked when MMWLAN is inactive (i.e., STA mode not enabled).
 *
 * @note This feature must be used along side listen interval. The feature will take no effect
 * without it.
 *
 * @param non_tim_mode_enabled  Boolean value indicating whether non-TIM mode support should be
 * enabled.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_set_non_tim_mode_enabled(bool non_tim_mode_enabled);

/**
 * Enumeration of Target Wake Time (TWT) modes.
 *
 * @note TWT is only supported as a requester.
 */
enum mmwlan_twt_mode
{
    /** TWT disabled */
    MMWLAN_TWT_DISABLED,
    /** TWT enabled as a requester */
    MMWLAN_TWT_REQUESTER,
    /** TWT enabled as a responder */
    MMWLAN_TWT_RESPONDER
};

/** Enumeration of Target Wake Time (TWT) setup commands. */
enum mmwlan_twt_setup_command
{
    /** TWT setup request command */
    MMWLAN_TWT_SETUP_REQUEST,
    /** TWT setup suggest command */
    MMWLAN_TWT_SETUP_SUGGEST,
    /** TWT setup demand command */
    MMWLAN_TWT_SETUP_DEMAND
};

/** Structure for storing Target Wake Time (TWT) configuration arguments. */
struct mmwlan_twt_config_args
{
    /** Target Wake Time (TWT) modes, @ref mmwlan_twt_mode. */
    enum mmwlan_twt_mode twt_mode;
    /**
     * TWT service period interval in micro seconds.
     * This parameter will be ignored if @c twt_wake_interval_mantissa or
     * @c twt_wake_interval_exponent is non-zero.
     */
    uint64_t twt_wake_interval_us;
    /**
     * TWT Wake interval mantissa
     * If non-zero, this parameter will be used to calculate @c twt_wake_interval_us.
     */
    uint16_t twt_wake_interval_mantissa;
    /**
     * TWT Wake interval exponent
     * If non-zero, this parameter will be used to calculate @c twt_wake_interval_us.
     */
    uint8_t twt_wake_interval_exponent;
    /** Minimum TWT wake duration in micro seconds. */
    uint32_t twt_min_wake_duration_us;
    /** TWT setup command, @ref mmwlan_twt_setup_command. */
    enum mmwlan_twt_setup_command twt_setup_command;
};

/**
 * Initializer for @ref mmwlan_twt_config_args.
 *
 * For example:
 *
 * @code{c}
 * struct mmwlan_twt_config_args twt_config_args = MMWLAN_TWT_CONFIG_ARGS_INIT;
 * @endcode
 */
#define MMWLAN_TWT_CONFIG_ARGS_INIT                                   \
    {                                                                 \
        .twt_mode = MMWLAN_TWT_DISABLED,                              \
        .twt_wake_interval_us = DEFAULT_TWT_WAKE_INTERVAL_US,         \
        .twt_wake_interval_mantissa = 0,                              \
        .twt_wake_interval_exponent = 0,                              \
        .twt_min_wake_duration_us = DEFAULT_TWT_MIN_WAKE_DURATION_US, \
        .twt_setup_command = MMWLAN_TWT_SETUP_REQUEST,                \
    }

/**
 * Add configurations for Target Wake Time (TWT).
 *
 * @note This is used to add TWT configuration for a new TWT agreement.
 *       This function must be invoked before @ref mmwlan_sta_enable.
 *
 * @param twt_config_args TWT configuration arguments @ref mmwlan_twt_config_args.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_twt_add_configuration(
    const struct mmwlan_twt_config_args *twt_config_args);

/**
 * Gets the station's AID
 *
 * @return AID of station, or 0 if not associated
 */
uint16_t mmwlan_get_aid(void);

/**
 * Gets the BSSID of the AP to which the STA is associated.
 *
 * @param bssid  Buffer to receive the BSSID. Length must be @ref MMWLAN_MAC_ADDR_LEN.
 *               Will only be set if @c MMWLAN_SUCCESS is returned.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_get_bssid(uint8_t *bssid);

/**
 * Gets the RSSI measured from the AP.
 *
 * When power save is enabled, this will only be updated when traffic is received from the AP.
 *
 * @returns last known RSSI of the AP or INT32_MIN on error.
 */
int32_t mmwlan_get_rssi(void);

/**
 * Sets the listen interval to be indicated in the association response frame. This informs the AP
 * how often the STA will wake up. The AP uses the listen interval in determining the lifetime of
 * frames that it buffers for a STA.
 *
 * @param[in] interval Interval value in beacon or short beacon units. e.g. 5 will set the listen
 *            interval to the interval between 5 beacons. 0 will disable this feature.
 *
 * @returns @c MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_set_listen_interval(uint16_t interval);

/**
 * @defgroup MMWLAN_OFFLOAD     [Deprecated] WLAN offload features
 *
 * @{
 *
 * WLAN offload features enable offloading some high level networking features to the WLAN chip.
 * Features like ARP response, ARP refresh and DHCP lease updates can be offloaded to the chip
 * allowing the host processor to sleep for longer resulting in better power savings.
 *
 * @deprecated It is not recommended to use the WLAN Offload API. It is deprecated and
 *             will be removed in a future release.
 */

/**
 * Enables ARP response offload.
 *
 * When enabled the chip will automatically respond to ARP requests with the specified
 * IPv4 address.
 *
 * @note ARP offload can only be enabled for a STA with an active connection.
 * @note ARP offload is not supported for IPv6 addresses.
 *
 * @param arp_addr  The IPv4 address to respond with for ARP requests for this interface.
 *
 * @returns @ref MMWLAN_SUCCESS on success, @ref MMWLAN_UNAVAILABLE if the chip does not have a
 *          valid connection, or @ref MMWLAN_ERROR on failure.
 *
 * @deprecated It is not recommended to use the WLAN Offload API. It is deprecated and
 *             will be removed in a future release.
 */
enum mmwlan_status mmwlan_enable_arp_response_offload(uint32_t arp_addr);

/**
 * Enables ARP refresh offload.
 *
 * When enabled the Morse chip will periodically send ARP requests to the AP to refresh
 * its ARP table. This keeps this stations ARP entry from expiring. ARP response offload
 * needs to be enabled first for this feature to work.
 *
 * @note ARP refresh offload is not supported for IPv6 addresses.
 *
 * @param interval_s   The interval in seconds to refresh the ARP entries on the AP.
 * @param dest_ip      The IP to send the ARP packets to.
 * @param send_as_garp If true, send as gratuitous ARP.
 *
 * @returns @ref MMWLAN_SUCCESS on success or @ref MMWLAN_ERROR on failure.
 *
 * @deprecated It is not recommended to use the WLAN Offload API. It is deprecated and
 *             will be removed in a future release.
 */
enum mmwlan_status mmwlan_enable_arp_refresh_offload(uint32_t interval_s,
                                                     uint32_t dest_ip,
                                                     bool send_as_garp);

/**
 * DHCP lease info structure.
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
struct mmwlan_dhcp_lease_info
{
    /** local IP address */
    uint32_t ip4_addr;
    /** Netmask address */
    uint32_t mask4_addr;
    /** Gateway address */
    uint32_t gw4_addr;
    /** DNS address */
    uint32_t dns4_addr;
};

/**
 * DHCP Lease update callback function prototype - this is called whenever a lease is updated.
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */

typedef void (*mmwlan_dhcp_lease_update_cb_t)(const struct mmwlan_dhcp_lease_info *lease_info,
                                              void *arg);

/**
 * Enables DHCP offload.
 *
 * When enabled the Morse chip will handle DHCP discovery and lease updates automatically.
 *
 * @note DHCP offload is not supported for IPv6 addresses.
 *
 * @note The DHCP lease update callback (@p dhcp_lease_update_cb) must not block and MMWLAN API
 *       functions may not be invoked from the callback.
 *
 * @note Once enabled this feature can only be disabled by a complete system reset or by calling
 *       @c mmwlan_shutdown().
 *
 * @param dhcp_lease_update_cb The callback to call whenever the DHCP lease is updated.
 * @param arg                  An opaque argument to pass to @c dhcp_lease_update_cb.
 *
 * @returns @ref MMWLAN_SUCCESS on success or @ref MMWLAN_ERROR on failure.
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
enum mmwlan_status mmwlan_enable_dhcp_offload(mmwlan_dhcp_lease_update_cb_t dhcp_lease_update_cb,
                                              void *arg);

/**
 * Keep-alive offload configuration options for @ref mmwlan_tcp_keepalive_offload_args.set_cfgs
 * bitmap.
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
enum mmwlan_tcp_keepalive_offload_cfg
{
    /** Bitmap for TCP keep alive period parameter */
    MMWLAN_TCP_KEEPALIVE_SET_CFG_PERIOD = (0x01),
    /** Bitmap for TCP keep alive retry count parameter */
    MMWLAN_TCP_KEEPALIVE_SET_CFG_RETRY_COUNT = (0x02),
    /** Bitmap for TCP keep alive retry interval parameter */
    MMWLAN_TCP_KEEPALIVE_SET_CFG_RETRY_INTERVAL = (0x04),
    /** Bitmap for TCP keep alive source IP parameter */
    MMWLAN_TCP_KEEPALIVE_SET_CFG_SRC_IP_ADDR = (0x08),
    /** Bitmap for TCP keep alive destination IP parameter */
    MMWLAN_TCP_KEEPALIVE_SET_CFG_DEST_IP_ADDR = (0x10),
    /** Bitmap for TCP keep alive source port parameter */
    MMWLAN_TCP_KEEPALIVE_SET_CFG_SRC_PORT = (0x20),
    /** Bitmap for TCP keep alive destination port parameter */
    MMWLAN_TCP_KEEPALIVE_SET_CFG_DEST_PORT = (0x40),
    /** Bitmap for TCP keep alive timing parameters only */
    MMWLAN_TCP_KEEPALIVE_SET_CFG_TIMING_ONLY = (0x07),
    /** Bitmap for all TCP keep alive parameters */
    MMWLAN_TCP_KEEPALIVE_SET_CFG_ALL = (0x7F),
};

/**
 * Arguments data structure for TCP keep-alive arguments.
 *
 * This structure should be initialized using @ref MMWLAN_TCP_KEEPALIVE_OFFLOAD_ARGS_INIT
 * for sensible default values, particularly for forward compatibility with new releases
 * that may add new fields to the struct. For example:
 *
 * @code{.c}
 * struct mmwlan_tcp_keepalive_offload_args args = MMWLAN_TCP_KEEPALIVE_OFFLOAD_ARGS_INIT;
 * @endcode
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
struct mmwlan_tcp_keepalive_offload_args
{
    /**
     * A bitmap specifying which configs to set.
     * See @ref mmwlan_tcp_keepalive_offload_cfg for values.
     */
    uint8_t set_cfgs;
    /**
     * The interval in seconds to send the keep alive in. @c MMWLAN_TCP_KEEPALIVE_SET_CFG_PERIOD
     * bit in @ref set_cfgs must be set if specifying this parameter
     */
    uint16_t period_s;
    /** Number of times to retry before giving up. @c MMWLAN_TCP_KEEPALIVE_SET_CFG_RETRY_COUNT
     * bit in @ref set_cfgs must be set if specifying this parameter
     */
    uint8_t retry_count;
    /** The time to wait between retries in seconds. @c MMWLAN_TCP_KEEPALIVE_SET_CFG_RETRY_INTERVAL
     * bit in @ref set_cfgs must be set if specifying this parameter
     */
    uint8_t retry_interval_s;
    /** The source IP for the keep alive packet. @c MMWLAN_TCP_KEEPALIVE_SET_CFG_SRC_IP_ADDR
     * bit in @ref set_cfgs must be set if specifying this parameter
     */
    uint32_t src_ip;
    /** The source port for the keep alive packet. @c MMWLAN_TCP_KEEPALIVE_SET_CFG_SRC_PORT
     * bit in @ref set_cfgs must be set if specifying this parameter
     */
    uint16_t src_port;
    /** The destination IP for the keep alive packet. @c MMWLAN_TCP_KEEPALIVE_SET_CFG_DEST_IP_ADDR
     * bit in @ref set_cfgs must be set if specifying this parameter
     */
    uint32_t dest_ip;
    /** The destination port for the keep alive packet. @c MMWLAN_TCP_KEEPALIVE_SET_CFG_SRC_PORT
     * bit in @ref set_cfgs must be set if specifying this parameter
     */
    uint16_t dest_port;
};

/**
 * Initializer for @ref mmwlan_tcp_keepalive_offload_args.
 *
 * @see mmwlan_tcp_keepalive_offload_args
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
#define MMWLAN_TCP_KEEPALIVE_OFFLOAD_ARGS_INIT { 0 }

/**
 * Enables TCP keep-alive offload.
 *
 * When enabled the Morse chip will periodically send TCP keep-alive packets to the destination
 * even when the host processor is in standby mode. This function needs to be called before
 * opening a TCP connection.
 *
 * @note TCP keep-alive offload will only be applied to the first TCP connection matching the
 *       given configuration that is opened after this function is called.
 *
 * @note TCP keep-alive offload is not supported for IPv6 addresses.
 *
 * @param args A pointer to arguments of @c struct @c mmwlan_tcp_keepalive_offload_args.
 *
 * @returns @ref MMWLAN_SUCCESS on success or @ref MMWLAN_ERROR on failure.
 */
enum mmwlan_status mmwlan_enable_tcp_keepalive_offload(
    const struct mmwlan_tcp_keepalive_offload_args *args);

/**
 * Disables the TCP keep-alive offload feature.
 *
 * Has no effect if TCP keep-alive offload is not enabled.
 *
 * @returns @ref MMWLAN_SUCCESS on success or @ref MMWLAN_ERROR on failure.
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
enum mmwlan_status mmwlan_disable_tcp_keepalive_offload(void);

/**
 * If this bit is set in the flags parameter of @ref mmwlan_config_whitelist then any active
 * whitelist filters are cleared,
 */
#define MMWLAN_WHITELIST_FLAGS_CLEAR 0x01

/**
 * Whitelist filter configuration
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
struct mmwlan_config_whitelist
{
    /** Flags - clear any active whitelist filters if bit 0 is set */
    uint8_t flags;
    /** The IP protocol to match - 6 for TCP, 17 for UDP, 0 for any */
    uint8_t ip_protocol;
    /** The @c LLC protocol to match - 0x0800 for IPv4 and 0x86DD for IPv6, 0 for any */
    uint16_t llc_protocol;
    /** The IPv4 source address to match, 0.0.0.0 for any. */
    uint32_t src_ip;
    /** The IPv4 destination address to match, 0.0.0.0 for any - this is usually our IP address */
    uint32_t dest_ip;
    /** The netmask to apply to the source or destination IP, 0.0.0.0 for any */
    uint32_t netmask;
    /** The source TCP or UDP port to match, 0 for any */
    uint16_t src_port;
    /** The destination TCP or UDP port to match, 0 for any */
    uint16_t dest_port;
};

/**
 * Sets and enables the IP whitelist filter.
 *
 * The IP whitelist filter specifies which incoming IP packets can wake the host from standby.
 * The filter can be used to specify parameters such as source or destination IP addresses to
 * match on, source or destination ports and even IP layer (Such as TCP, ICMP or UDP) or @c LLC
 * layer (Such as IPv4 or IPv6) protocols. If any filter parameter is set to 0, then it is
 * excluded from the filtering process.
 *
 * If the @c flags parameter is set to @c MMWLAN_WHITELIST_FLAGS_CLEAR, then all active whitelist
 * filters are cleared.
 *
 * @note Address/port level filtering is not supported for IPv6 packets.
 *
 * @param whitelist The whitelist filter to set.
 *
 * @returns @ref MMWLAN_SUCCESS on success or @ref MMWLAN_ERROR on failure.
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
enum mmwlan_status mmwlan_set_whitelist_filter(const struct mmwlan_config_whitelist *whitelist);

/** @} */

/**
 * @defgroup MMWLAN_STANDBY     [Deprecated] WLAN Standby features
 *
 * @ingroup MMWLAN_OFFLOAD
 *
 * @{
 *
 * The standby mode allows the system to power off or put the host processor in a deep sleep mode
 * while the Morse chip takes over certain functionality to keep the connection alive with the
 * provision to wake up the host processor when certain conditions are met.
 *
 * When in Standby mode, the chip will:
 * - Not try to communicate with the host in any way.
 * - Use normal Power Save mechanisms to wake for DTIM beacons and to receive data frames when a
 *   DTIM beacon indicates pending data.
 * - Directly respond to ARP requests if ARP offload is enabled, handle DHCP lease expiry/renewal
 *   if DHCP offload is enabled, and drop all other traffic other than a wake request packet from
 *   the access point.
 * - Monitor access point reach-ability and provide status by periodically transmitting encrypted
 *   keep-alive frames to the access point.
 * - If access point is no longer reachable, then the chip will enter a snooze mode where
 *   it stops listening for beacons, waking up only occasionally to see if the access point is
 *   reachable again. This allows the chip to conserve power when the access point is not reachable.
 *
 * The chip will wake the host only if one of the following occurs.
 * - An encrypted Standby wake packet is received.
 * - Association has been lost (indicated via a @c deauth response to a keep-alive frame from the
 *   access point).
 * - The access point was not reachable and beacons are now detected indicating the access point
 *   is available again.
 * - The input trigger GPIO pin on the chip was toggled indicating some on board hardware requires
 *   attention.
 * - A TCP connection maintained by the TCP keep-alive offload feature was lost.
 *
 * The host will be powered on by toggling a GPIO pin, the BUSY pin is also toggled to interrupt
 * the host. By offloading features like ARP response, ARP refresh, TCP keep-alive, and DHCP lease
 * updates while in standby mode allows the host processor to sleep for longer resulting in better
 * power savings.
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */

/**
 * The maximum length of a user-specified payload (bytes) for Standby status
 * frames.
 */
#define MMWLAN_STANDBY_STATUS_FRAME_USER_PAYLOAD_MAXLEN (64)

/** The maximum allowed length of a user filter to apply to wake frames */
#define MMWLAN_STANDBY_WAKE_FRAME_USER_FILTER_MAXLEN (64)

/**
 * Reasons we can exit standby mode.
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
enum mmwlan_standby_exit_reason
{
    /** No specific reason for exiting standby mode */
    MMWLAN_STANDBY_EXIT_REASON_NONE,
    /** The STA has received the wake-up frame */
    MMWLAN_STANDBY_EXIT_REASON_WAKEUP_FRAME,
    /** The STA needs to (re)associate */
    MMWLAN_STANDBY_EXIT_REASON_ASSOCIATE,
    /** The external input pin has fired */
    MMWLAN_STANDBY_EXIT_REASON_EXT_INPUT,
    /** Whitelisted packet received */
    MMWLAN_STANDBY_EXIT_REASON_WHITELIST_PKT,
    /** TCP connection lost */
    MMWLAN_STANDBY_EXIT_REASON_TCP_CONNECTION_LOST,
    /** HW scan is not enabled */
    MMWLAN_STANDBY_EXIT_REASON_HW_SCAN_NOT_ENABLED,
    /** HW scan failed to start */
    MMWLAN_STANDBY_EXIT_REASON_HW_SCAN_FAILED_TO_START,
};

/**
 * Standby mode callback whenever an event requiring an exit from standby mode occurs.
 *
 * @param reason The reason we exited standby mode. See enum @ref mmwlan_standby_exit_reason.
 * @param arg    An opaque pointer passed from @ref mmwlan_standby_enter()
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
typedef void (*mmwlan_standby_exit_cb_t)(uint8_t reason, void *arg);

/**
 * Arguments for @ref mmwlan_standby_enter
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
struct mmwlan_standby_enter_args
{
    /** Callback function to call when we exit standby mode */
    mmwlan_standby_exit_cb_t standby_exit_cb;
    /** Arguments to pass to callback function */
    void *standby_exit_arg;
};

/**
 * This function puts the Morse chip into standby mode allowing the host processor to go to sleep.
 *
 * When in standby mode the Morse chip takes over certain functionality to keep the connection
 * alive with the provision to wake up the host processor when certain conditions are met.
 * Exit from standby mode can be triggered by the Morse chip under certain conditions or by the host
 * by invoking @ref mmwlan_standby_exit(). Before invoking this function, standby mode parameters
 * can be configured by calling @ref mmwlan_standby_set_config(), @ref
 * mmwlan_standby_set_status_payload() and/or
 * @ref mmwlan_standby_set_wake_filter()
 *
 * @param args   A pointer to the arguments for this function.
 *
 * @returns @ref MMWLAN_SUCCESS on success or @ref MMWLAN_ERROR on failure.
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
enum mmwlan_status mmwlan_standby_enter(const struct mmwlan_standby_enter_args *args);

/**
 * Forces the Morse chip to exit standby mode.
 *
 * here may be certain instances such as a timer expiry, which cause the host chip to wake up
 * independent of the Morse chip. In such situations, the host calls this function to instruct
 * the Morse chip to exit standby mode and return to normal operating mode.
 *
 * Triggers @ref mmwlan_standby_exit_cb_t with reason @c MMWLAN_STANDBY_EXIT_REASON_NONE.
 *
 * @returns @ref MMWLAN_SUCCESS on success or @ref MMWLAN_ERROR on failure.
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
enum mmwlan_status mmwlan_standby_exit(void);

/**
 * Arguments for @ref mmwlan_standby_set_status_payload.
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
struct mmwlan_standby_set_status_payload_args
{
    /** User data to append to the standby status packets */
    uint8_t payload[MMWLAN_STANDBY_STATUS_FRAME_USER_PAYLOAD_MAXLEN];
    /**
     * The length of the payload in bytes.
     * See @ref MMWLAN_STANDBY_STATUS_FRAME_USER_PAYLOAD_MAXLEN for maximum payload length.
     */
    uint32_t payload_len;
};

/**
 * Sets the user payload in the standby status packet.
 *
 * Once standby mode is enabled, the Morse chip will periodically emit a UDP packet of the
 * following format regardless of whether it is in standby or not. The UDP packet
 * will also be sent immediately upon entering or exiting Standby mode.
 *
 * @code
 * +----------------------+----------------------+--------------------------------+---------+
 * | Morse OUI (0c:bf:74) | Type: Standby (0x01) | Awake (0x00) or Standby (0x01) | Payload |
 * +----------------------+----------------------+--------------------------------+---------+
 * @endcode
 *
 * @note The payload is optional and is not present if this function is not called.
 *
 * @param args   A pointer to the arguments for this function.
 *
 * @returns @ref MMWLAN_SUCCESS on success or @ref MMWLAN_ERROR on failure.
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
enum mmwlan_status mmwlan_standby_set_status_payload(
    const struct mmwlan_standby_set_status_payload_args *args);

/**
 * Arguments for @ref mmwlan_standby_set_wake_filter.
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
struct mmwlan_standby_set_wake_filter_args
{
    /** Data to match with for filtering */
    uint8_t filter[MMWLAN_STANDBY_WAKE_FRAME_USER_FILTER_MAXLEN];
    /**
     * The length of the filter data in bytes.
     * See @ref MMWLAN_STANDBY_WAKE_FRAME_USER_FILTER_MAXLEN for maximum filter length.
     */
    uint32_t filter_len;
    /** The offset within the packet to search for the filter match */
    uint32_t offset;
};

/**
 * Configures the standby mode UDP wake packet filter.
 *
 * The system can be woken up from standby mode by sending it a UDP wake packet of the following
 * format. If a wake filter is set using this function then the wake packet will only wake up
 * the system if the specified filter pattern matches the payload at the specified offset within
 * the payload.
 *
 * The wake packet has the following format:
 * @code
 * +----------------------+----------------------+----------------+--------------------+
 * | Morse OUI (0c:bf:74) | Type: Standby (0x01) | Wake up (0x02) | Payload (optional) |
 * +----------------------+----------------------+----------------+--------------------+
 * @endcode
 *
 * @note If a wake filter is not configured then the system will wake on any wake packet and
 *       the payload (if any) is ignored.
 *
 * @param args   A pointer to the arguments for this function.
 *
 * @returns @ref MMWLAN_SUCCESS on success or @ref MMWLAN_ERROR on failure.
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
enum mmwlan_status mmwlan_standby_set_wake_filter(
    const struct mmwlan_standby_set_wake_filter_args *args);

/**
 * Standby mode configuration parameters. If the @ref mmwlan_standby_set_config()
 * function is not called, then the defaults are as specified.
 */
struct mmwlan_standby_config
{
    /** Interval in seconds for transmitting Standby status packets. (Default 15s) */
    uint32_t notify_period_s;
    /** Source IP address to use for the standby status packets. (Default 0.0.0.0) */
    uint32_t src_ip;
    /** Destination IP address for the standby status packets. (Default 0.0.0.0) */
    uint32_t dst_ip;
    /**
     * Destination UDP Port for the standby status packets, also used the source port for
     * outgoing UDP port for outgoing UDP packets. (Default 22000)
     */
    uint16_t dst_port;
    /**
     * @deprecated This parameter is no longer used and will be removed in a future release.
     */
    uint32_t bss_inactivity_before_snooze_s;
    /**
     * The interval in seconds to wake periodically from snooze and check for beacons.
     * If no beacons are found then the chip will re-enter snooze. If beacons are found
     * then the chip will exit standby mode so the host can re-associate. (Default 60s)
     */
    uint32_t snooze_period_s;
    /**
     * The amount in seconds to increase successive snooze intervals. This saves power
     * by sleeping for longer before checking for beacons again if no beacons are found.
     * (Default 0s)
     */
    uint32_t snooze_increment_s;
    /** The maximum time in seconds to snooze for after increments. (Default @c UINT32_MAX) */
    uint32_t snooze_max_s;
};

/**
 * Sets the configuration for standby mode.
 *
 * @param config A pointer to the configuration structure. See @ref mmwlan_standby_config.
 *
 * @returns @ref MMWLAN_SUCCESS on success or @ref MMWLAN_ERROR on failure.
 *
 * @deprecated It is not recommended to use the WLAN Offload features API. It is deprecated and
 *             will be removed in a future release.
 */
enum mmwlan_status mmwlan_standby_set_config(const struct mmwlan_standby_config *config);

/** @} */

/**
 * @defgroup MMWLAN_WNM     WNM Sleep management
 *
 * @{
 *
 * @note It is highly recommended to refer to Morse Micro App Note 04 on WNM sleep for a more
 *       detailed explanation on how this feature works.
 *
 * WNM sleep is an extended power save mode in which the STA need not listen for every DTIM beacon
 * and need not perform rekey, allowing the STA to remain associated and reduce power consumption
 * when it has no traffic to send or receive from the AP. While a STA is in WNM sleep it will be
 * unable to communicate with the AP, and thus any traffic for the STA may be buffered or dropped
 * at the discretion of the AP while the STA is in WNM sleep.
 *
 * @note WNM sleep can only be entered after the STA is associated to the AP. The AP must also
 *       support WNM sleep.
 *
 * When entry to WNM sleep is enabled via this API, the STA sends a request to the AP and after
 * successful negotiation, the chip is either put into a lower power mode or powered down
 * completely, depending on arguments given when WNM sleep mode was enabled. The datapath is paused
 * during this time. If the STA does not successfully negotiate with the AP to enter WNM Sleep, it
 * will return an error code and the chip will not enter a low power mode.
 *
 * When WNM sleep is disabled, the chip returns to normal operation and sends a request to the AP
 * to exit WNM sleep. Note that the chip will exit low power mode regardless of whether or not the
 * exit request was successful. WNM sleep will be exited if @ref mmwlan_shutdown or
 * @ref mmwlan_sta_disable is invoked.
 *
 * If the AP takes down the link, then the station will be unaware until it sends the WNM Sleep
 * exit request. At that point the AP will send a de-authentication frame with reason code
 * "non-associated station". After validating this is actually the AP the station was connected
 * to, the station will bring down the connection and return @ref MMWLAN_ERROR. The station will
 * attempt to re-associate but will not re-enter WNM sleep. The application needs to enable WNM
 * sleep again via this API if required.
 *
 * A high level overview of enabling and disabling WNM sleep is shown below.
 *
 * @include{doc} wnm_sleep_overview.puml
 */

/** Structure for storing WNM sleep extended arguments. */
struct mmwlan_set_wnm_sleep_enabled_args
{
    /** Boolean indicating whether WNM sleep is enabled. */
    bool wnm_sleep_enabled;
    /** Boolean indicating whether chip should be powered down during WNM sleep. */
    bool chip_powerdown_enabled;
};

/**
 * Initializer for @ref mmwlan_set_wnm_sleep_enabled_args.
 *
 * For example:
 *
 * @code{c}
 * struct mmwlan_set_wnm_sleep_enabled_args wnm_sleep_args = MMWLAN_SET_WNM_SLEEP_ENABLED_ARGS_INIT;
 * @endcode
 */
#define MMWLAN_SET_WNM_SLEEP_ENABLED_ARGS_INIT { false, false }

/**
 * Sets extended WNM sleep mode.
 *
 * Provides an extended interface for setting WNM sleep. See @ref mmwlan_set_wnm_sleep_enabled_args
 * for parameter details. If WNM sleep mode is enabled then the transceiver will sleep across
 * multiple DTIM periods until WNM sleep mode is disabled. This allows the transceiver to use less
 * power with the caveat that it will not wake up for group- or individually-addressed traffic. If
 * a group rekey occurs while the device is in WNM sleep it will be applied when the device exits
 * WNM sleep.
 *
 * @note Data should not be queued for transmission (using, e.g., @ref mmwlan_tx())  during
 *       WNM sleep.
 * @note 802.11 power save must be enabled for any benefit to be obtained
 *       (see @ref mmwlan_set_power_save_mode()).
 * @note Negotiation with the AP is required to enter WNM sleep. As such the transceiver will only
 *       be placed into low power mode when a connection has been establish and the AP has accepted
 *       the request to enter WNM sleep. This will automatically be handled within mmwlan once WNM
 *       sleep mode is enabled.
 *
 * @param args  WNM sleep arguments - see @ref mmwlan_set_wnm_sleep_enabled_args.
 *
 * @return @ref MMWLAN_SUCCESS on success, MMWLAN_UNAVAILABLE if already requested or if not
 *              currently connected, else an appropriate error code.
 *         @ref MMWLAN_TIMED_OUT if the maximum retry request reached. For WNM sleep exit request,
 *              this means that the device exited WNM sleep but failed to inform the AP.
 */
enum mmwlan_status mmwlan_set_wnm_sleep_enabled_ext(
    const struct mmwlan_set_wnm_sleep_enabled_args *args);

/**
 * Sets whether WNM sleep mode is enabled.
 *
 * If WNM sleep mode is enabled then the transceiver will sleep across multiple DTIM periods
 * until WNM sleep mode is disabled. This allows the transceiver to use less power with the
 * caveat that it will not wake up for group- or individually-addressed traffic. If a group
 * rekey occurs while the device is in WNM sleep it will be applied when the device exits
 * WNM sleep.
 *
 * @note Data should not be queued for transmission (using, e.g., @ref mmwlan_tx())  during
 *       WNM sleep.
 * @note 802.11 power save must be enabled for any benefit to be obtained
 *       (see @ref mmwlan_set_power_save_mode()).
 * @note Negotiation with the AP is required to enter WNM sleep. As such the transceiver will only
 *       be placed into low power mode when a connection has been establish and the AP has accepted
 *       the request to enter WNM sleep. This will automatically be handled within mmwlan once WNM
 *       sleep mode is enabled.
 *
 * @param wnm_sleep_enabled   Boolean indicating whether WNM sleep is enabled.
 *
 * @return @ref MMWLAN_SUCCESS on success, MMWLAN_UNAVAILABLE if already requested or if not
 *              currently connected, else an appropriate error code.
 *         @ref MMWLAN_TIMED_OUT if the maximum retry request reached. For WNM sleep exit request,
 *              this means that the device exited WNM sleep but failed to inform the AP.
 */
static inline enum mmwlan_status mmwlan_set_wnm_sleep_enabled(bool wnm_sleep_enabled)
{
    struct mmwlan_set_wnm_sleep_enabled_args wnm_sleep_args =
        MMWLAN_SET_WNM_SLEEP_ENABLED_ARGS_INIT;
    wnm_sleep_args.wnm_sleep_enabled = wnm_sleep_enabled;
    return mmwlan_set_wnm_sleep_enabled_ext(&wnm_sleep_args);
}

/** @} */
/** @} */

/**
 * @defgroup MMWLAN_DPP    WLAN Control API for Device Provisioning Protocol (DPP)
 *
 * @{
 * API for executing Device Provisioning Protocol (DPP), also known as Wi-Fi Easy Connect.
 */

/** Enumeration of DPP events. */
enum mmwlan_dpp_event
{
    /** DPP push button result. */
    MMWLAN_DPP_EVT_PB_RESULT,
};

/** Enumeration of results for @c MMWLAN_DPP_EVT_PB_RESULT. */
enum mmwlan_dpp_pb_result
{
    /** DPP push button process was successful. */
    MMWLAN_DPP_PB_RESULT_SUCCESS,
    /** An error occurred during the DPP push button process. */
    MMWLAN_DPP_PB_RESULT_ERROR,
    /** A session overlap occurred during the DPP push button process. */
    MMWLAN_DPP_PB_RESULT_SESSION_OVERLAP,
};

/**
 * Structure passed back when a DPP event occurs.
 *
 * @warning BETA NOTICE: This is beta API that is under development;
 *          breaking changes may be introduced in future releases.
 */
struct mmwlan_dpp_cb_args
{
    /** The DPP event that has occurred. */
    enum mmwlan_dpp_event event;

    /** Union of arguments for DPP events. */
    union
    {
        /** Argument for @c MMWLAN_DPP_EVT_PB_RESULT event. */
        struct
        {
            /** Result of DPP push button. */
            enum mmwlan_dpp_pb_result result;
            /** SSID of the AP to connect to. May be @c NUlL. */
            const uint8_t *ssid;
            /** Length of the SSID. */
            uint16_t ssid_len;
            /** Passphrase, NULL terminated. May be @c NULL. */
            const char *passphrase;
        } pb_result;
    } args;
};

/**
 * Structure to hold the arguments used for the DPP process.
 *
 * @warning BETA NOTICE: This is beta API that is under development;
 *          breaking changes may be introduced in future releases.
 */
struct mmwlan_dpp_args
{
    /** DPP event callback prototype. */
    void (*dpp_event_cb)(const struct mmwlan_dpp_cb_args *dpp_event, void *arg);
    /** Optional user argument that will be passed back to the DPP event callback. */
    void *dpp_event_cb_arg;
};

/**
 * Function to start the Device Provisioning Protocol (DPP) process. This will return once DPP has
 * successfully started. Feedback will be provided via the @c dpp_event_cb.
 *
 * @warning If this has been called @c mmwlan_dpp_stop() MUST be called before @c mmwlan_shutdown()
 * is called.
 *
 * @param args Reference to the dpp arguments to use.
 *
 * @returns @c MMWLAN_SUCCESS on success, else an appropriate error code.
 *
 * @warning BETA NOTICE: This is beta API that is under development;
 *          breaking changes may be introduced in future releases.
 */
enum mmwlan_status mmwlan_dpp_start(const struct mmwlan_dpp_args *args);

/**
 * Function to stop the DPP process.
 *
 * @returns @c MMWLAN_SUCCESS on success, else an appropriate error code.
 *
 * @warning BETA NOTICE: This is beta API that is under development;
 *          breaking changes may be introduced in future releases.
 */
enum mmwlan_status mmwlan_dpp_stop(void);

/** @} */

/**
 * @defgroup MMWLAN_AP    WLAN Control API for Access Point (AP) mode
 *
 * @{
 *
 * API for configuration and control of Access Point (AP) mode.
 *
 * @warning BETA NOTICE: This is beta API that is under development;
 *          breaking changes may be introduced in future releases.
 */

/** Default Beacon Interval in AP mode (in TUs). */
#define MMWLAN_DEFAULT_AP_BEACON_INTERVAL_TUS (100)

/** Default DTIM period in AP mode. */
#define MMWLAN_DEFAULT_AP_DTIM_PERIOD (1)

/** Default limit of connected stations */
#define MMWLAN_DEFAULT_AP_MAX_STAS (4)

/** Maximum limit of connected stations */
#define MMWLAN_AP_MAX_STAS_LIMIT (20)

/**
 * Enumeration of STA states.
 *
 * @warning BETA NOTICE: This is beta API that is under development;
 *          breaking changes may be introduced in future releases.
 */
enum mmwlan_ap_sta_state
{
    /** The STA is not known. */
    MMWLAN_AP_STA_UNKNOWN,
    /** The STA is associated but not yet authorized for data transmission. */
    MMWLAN_AP_STA_ASSOCIATED,
    /** The STA is fully connected and authorized for data transmission. */
    MMWLAN_AP_STA_AUTHORIZED,
};

/**
 * Data structure for communicating STA status information for stations connected to an AP.
 *
 * @warning BETA NOTICE: This is beta API that is under development;
 *          breaking changes may be introduced in future releases.
 */
struct mmwlan_ap_sta_status
{
    /** The current state of the STA. */
    enum mmwlan_ap_sta_state state;
    /** The AID of the STA. */
    uint16_t aid;
    /** The MAC address of the STA. */
    uint8_t mac_addr[MMWLAN_MAC_ADDR_LEN];
};

/**
 * Type definition for callback to be invoked on change in status of a connected STA.
 *
 * @param sta_status    The STA status information.
 * @param arg           Opaque argument that was provided when the callback was registered.
 *
 * @warning BETA NOTICE: This is beta API that is under development;
 *          breaking changes may be introduced in future releases.
 */
typedef void (*mmwlan_ap_sta_status_cb_t)(const struct mmwlan_ap_sta_status *sta_status, void *arg);

/**
 * Gets the STA status of the STA with the given MAC address.
 *
 * @param[in]  sta_addr     Address of the STA to get the status of.
 * @param[out] sta_status   STA status structure to be filled out by this function. May be @c NULL.
 *
 * @returns @ref MMWLAN_SUCCESS on success, @ref MMWLAN_NOT_FOUND if no STA record was found
 *          matching the given MAC address, or another error code as appropriate.

 * @warning BETA NOTICE: This is beta API that is under development;
 *          breaking changes may be introduced in future releases.
 */
enum mmwlan_status mmwlan_ap_get_sta_status(const uint8_t *sta_addr,
                                            struct mmwlan_ap_sta_status *sta_status);

/**
 * Arguments data structure for @ref mmwlan_ap_enable().
 *
 * This structure should be initialized using @ref MMWLAN_AP_ARGS_INIT for sensible
 * default values, particularly for forward compatibility with new releases that may add
 * new fields to the struct. For example:
 *
 * @code{.c}
 *     enum mmwlan_status status;
 *     struct mmwlan_ap_args ap_args = MMWLAN_AP_ARGS_INIT;
 *     // HERE: initialize arguments
 *     status = mmwlan_ap_enable(&ap_args);
 * @endcode
 *
 * @warning BETA NOTICE: This is beta API that is under development;
 *          breaking changes may be introduced in future releases.
 */
struct mmwlan_ap_args
{
    /** SSID of the AP. */
    uint8_t ssid[MMWLAN_SSID_MAXLEN];
    /** Length of the SSID. */
    uint16_t ssid_len;
    /**
     * Optional BSSID of the AP. If zero then the devices MAC address will be used.
     *
     * @warning The MAC address selection behavior may change in future.
     */
    uint8_t bssid[MMWLAN_MAC_ADDR_LEN];
    /** Type of security to use. If @c MMWLAN_SAE then a @c passphrase must be specified. */
    enum mmwlan_security_type security_type;
    /** Passphrase (only used if @c security_type is @c MMWLAN_SAE, otherwise ignored. */
    char passphrase[MMWLAN_PASSPHRASE_MAXLEN + 1];
    /** Length of @c passphrase. May be zero if @c passphrase is null-terminated. */
    uint16_t passphrase_len;
    /** Protected Management Frame mode to use (802.11w) */
    enum mmwlan_pmf_mode pmf_mode;
    /**
     * Preference list of enabled elliptic curve groups for SAE and OWE.
     * By default (if this parameter is not set), the mandatory group 19 is preferred.
     */
    int sae_owe_ec_groups[MMWLAN_MAX_EC_GROUPS];
    /**
     * Operating Class to use (S1G or Global).
     *
     * The combination of this field and @c s1g_chan_num will be used
     * to look up the appropriate entry in the channel list, which must have been previously
     * provided using @ref mmwlan_set_channel_list().
     */
    uint16_t op_class;
    /**
     * S1G channel number of the channel to use.
     *
     * The combination of this field and @c op_class will be used
     * to look up the appropriate entry in the channel list, which must have been previously
     * provided using @ref mmwlan_set_channel_list().
     */
    uint16_t s1g_chan_num;
    /**
     * The Beacon period in units of TUs. A TU is equal to 1.024 ms.
     *
     * If zero then the default value, @ref MMWLAN_DEFAULT_AP_BEACON_INTERVAL_TUS, will be used.
     */
    uint16_t beacon_interval_tus;
    /**
     * The Delivery Traffic Indication Map (DTIM) interval in beacons.
     *
     * If zero then the default value, @ref MMWLAN_DEFAULT_AP_DTIM_PERIOD, will be used.
     */
    uint16_t dtim_period;
    /**
     * Bandwidth to use for the primary channel. This may be set to 0 to automatically select
     * the highest primary bandwidth supported by the operating channel.
     *
     * @note This must not be greater than the bandwidth of the operating channel or 2 MHz,
     *       whichever is lower.
     */
    uint8_t pri_bw_mhz;
    /**
     * Index of the primary 1 Mhz channel within the operating channel. This must be less than
     * the bandwidth of the operating channel.
     */
    uint8_t pri_1mhz_chan_idx;
    /**
     * Optional callback to be invoked when the status of a connected STA changes. May be
     * set to @c NULL.
     */
    mmwlan_ap_sta_status_cb_t sta_status_cb;
    /**
     * Optional opaque argument to be passed to @c sta_status_cb. May optionally be @c NULL.
     * The value of this parameter must remain valid during the lifetime of the AP.
     */
    void *sta_status_cb_arg;
    /**
     * Maximum number of stations that can connect to the AP simultaneously. The maximum value limit
     * is @ref MMWLAN_AP_MAX_STAS_LIMIT.
     *
     * If zero, the default value of @ref MMWLAN_DEFAULT_AP_MAX_STAS will be used.
     */
    uint8_t max_stas;
};

/**
 * Initializer for @ref mmwlan_ap_args.
 *
 * @see mmwlan_ap_args
 */
#define MMWLAN_AP_ARGS_INIT              \
    {                                    \
        .ssid = { 0 },                   \
        .ssid_len = 0,                   \
        .bssid = { 0 },                  \
        .security_type = MMWLAN_OPEN,    \
        .passphrase = { 0 },             \
        .passphrase_len = 0,             \
        .pmf_mode = MMWLAN_PMF_REQUIRED, \
        .sae_owe_ec_groups = { 0 },      \
        .op_class = 0,                   \
        .s1g_chan_num = 0,               \
        .beacon_interval_tus = 0,        \
        .dtim_period = 0,                \
        .pri_bw_mhz = 0,                 \
        .pri_1mhz_chan_idx = 0,          \
        .sta_status_cb = NULL,           \
        .sta_status_cb_arg = NULL,       \
        .max_stas = 0,                   \
    }

/**
 * Enable AP mode.
 *
 * This will power on the transceiver then start Access Point mode.
 *
 * @warning Channel list must be set before enabling station mode. @ref mmwlan_set_channel_list().
 *
 * @warning OWE security is not currently supported for AP mode.
 *
 * @param args              Arguments (e.g., SSID, etc.). See @ref mmwlan_ap_args.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 *
 * @warning BETA NOTICE: This is beta API that is under development;
 *          breaking changes may be introduced in future releases.
 */
enum mmwlan_status mmwlan_ap_enable(const struct mmwlan_ap_args *args);

/**
 * Disable AP mode.
 *
 * This will disconnect any stations from the AP. It will also shut down the transceiver if nothing
 * else is holding it open. Note that if the transceiver was booted by @c mmwlan_boot() then
 * this function will not shut down the transceiver.
 *
 * @return @ref MMWLAN_SUCCESS if successful and the transceiver was also shut down,
 *         @ref MMWLAN_SHUTDOWN_BLOCKED if successful and the transceiver was not shut down,
 *         else an appropriate error code.
 *
 * @warning BETA NOTICE: This is beta API that is under development;
 *          breaking changes may be introduced in future releases.
 */
enum mmwlan_status mmwlan_ap_disable(void);

/**
 * Gets the BSSID address of the AP, if it is active.
 *
 * @param bssid  The BSSID of the AP. Length must be @ref MMWLAN_MAC_ADDR_LEN.
 *
 * @return @ref MMWLAN_SUCCESS on success, @ref MMWLAN_UNAVAILABLE if AP mode is not active.
 */
enum mmwlan_status mmwlan_ap_get_bssid(uint8_t *bssid);

/** @} */

/**
 * @defgroup MMWLAN_BEACON_VENDOR_IE_FILTER_API     WLAN Beacon Vendor Specific IE Filter API
 *
 * @{
 *
 * API for accessing Vendor Specific information elements (IEs) in beacons. The application
 * can register a callback to be executed if Vendor Specific IEs containing one of the specified
 * Organizational Unique Identifiers (OUIs). Up to @ref MMWLAN_BEACON_VENDOR_IE_MAX_OUI_FILTERS can
 * be specified.
 */

/** Max number of OUIs supported in vendor IE OUI filter, @ref mmwlan_beacon_vendor_ie_filter. */
#define MMWLAN_BEACON_VENDOR_IE_MAX_OUI_FILTERS (5)

/**
 * Beacon vendor ie filter callback function prototype.
 *
 * @param ies       Reference to the list of information elements (little endian order) present in
 *                  the beacon that matched the filter.
 * @param ies_len   Length of the IE list in octets.
 * @param arg       Reference to the opaque argument provided with the filter.
 */
typedef void (
    *mmwlan_beacon_vendor_ie_filter_cb_t)(const uint8_t *ies, uint32_t ies_len, void *arg);

/** 24-bit OUI type. */
typedef uint8_t mmwlan_oui_t[MMWLAN_OUI_SIZE];

/** Structure for storing beacon vendor ie filter parameter. */
struct mmwlan_beacon_vendor_ie_filter
{
    /**
     * Callback that will be executed upon reception of a beacon containing a Vendor Specific
     * information element that has an OUI that matches this filter. Must not be @c NULL.
     *
     * @note This is executed in the MMWLAN main processing loop. As such, processing within the
     * callback itself should be kept as short as possible. If extensive processing is done within
     * the callback this could have an impact on throughput and/or connection stability.
     */
    mmwlan_beacon_vendor_ie_filter_cb_t cb;
    /** Opaque argument to be passed to the callbacks. */
    void *cb_arg;
    /** Number of OUIs contained within @ref ouis. This can be @ref
     * MMWLAN_BEACON_VENDOR_IE_MAX_OUI_FILTERS max. */
    uint8_t n_ouis;
    /** List of OUIs to filter on. */
    mmwlan_oui_t ouis[MMWLAN_BEACON_VENDOR_IE_MAX_OUI_FILTERS];
};

/**
 * Function to update the filter used to pass beacon back.
 *
 * @note This will block until the filter has been installed or some error has occurred.
 *
 * @param filter  Reference to the filter to use. This must reside in memory and not be modified
 *                until after the filter is **successfully** cleared. To clear the filter pass @c
 *                NULL here.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_update_beacon_vendor_ie_filter(
    const struct mmwlan_beacon_vendor_ie_filter *filter);

/** @} */

/*
 * ---------------------------------------------------------------------------------------------
 */

/**
 * @defgroup MMWLAN_INIT    WLAN Initialization/Deinitialization API
 *
 * @note If using LWIP, this API is invoked by the LWIP @c netif for MMWLAN, and thus does not
 *       need to be used directly by the application.
 *
 * @{
 *
 * API for one-time initialization of MMWLAN.
 */

/**
 * Initialize the MMWLAN subsystem.
 *
 * @warning @ref mmhal_init() must be called before this function is executed.
 */
void mmwlan_init(void);

/**
 * Deinitialize the MMWLAN subsystem, freeing any allocated memory.
 *
 * Must not be called while there is any activity in progress (e.g., while connected).
 *
 * @warning @ref mmwlan_shutdown must be called before executing this function.
 */
void mmwlan_deinit(void);

/** @} */

/*
 * ---------------------------------------------------------------------------------------------
 */

/**
 * @defgroup MMWLAN_DATA    WLAN Datapath API
 *
 * @{
 *
 * Datapath API that is typically hooked up to the network stack.
 */

/** Enumeration of link states. */
enum mmwlan_link_state
{
    MMWLAN_LINK_DOWN, /**< The link is down. */
    MMWLAN_LINK_UP, /**< The link is up. */
};

/**
 * Prototype for link state change callbacks.
 *
 * @param link_state    The new link state.
 * @param arg           Opaque argument that was given when the callback was registered.
 */
typedef void (*mmwlan_link_state_cb_t)(enum mmwlan_link_state link_state, void *arg);

/**
 * Register a link status callback for use in STA mode.
 *
 * @deprecated This function is deprecated and provided for backwards compatibility.
 *             @ref mmwlan_register_vif_state_cb should be used for new developments.
 *
 * @note Only one link status callback may be registered. Further registration will overwrite the
 *       previously registered callback.
 *
 * @note The link status callback must not block and MMWLAN API functions may not be invoked
 *       from the callback.
 *
 * @note This link status callback will not be invoked in AP mode.
 *
 * @param callback  The callback to register.
 * @param arg       Opaque argument to be passed to the callback.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_register_link_state_cb(mmwlan_link_state_cb_t callback, void *arg);

/** VIF state used by @ref mmwlan_vif_state_cb_t. */
struct mmwlan_vif_state
{
    /** The VIF that this applies to. */
    enum mmwlan_vif vif;
    /** The current link state of the VIF. */
    enum mmwlan_link_state link_state;
};

/**
 * Prototype for VIF state change callbacks.
 *
 * @param state The new VIF state.
 * @param arg   Opaque argument that was given when the callback was registered.
 */
typedef void (*mmwlan_vif_state_cb_t)(const struct mmwlan_vif_state *state, void *arg);

/**
 * Register a VIF state callback.
 *
 * @note Only one VIF state callback may be registered per VIF. Further registration for a
 *       given VIF will overwrite the previously registered callback.
 *
 * @note The VIF state callback must not block and MMWLAN API functions may not be invoked
 *       from the callback.
 *
 * @param vif       The VIF that this callback applies to. If @ref MMWLAN_VIF_UNSPECIFIED then
 *                  the callback will be registered for all interfaces.
 * @param callback  The callback to register.
 * @param arg       Opaque argument to be passed to the callback.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_register_vif_state_cb(enum mmwlan_vif vif,
                                                mmwlan_vif_state_cb_t callback,
                                                void *arg);

/**
 * Receive data packet callback function.
 *
 * @param header        Buffer containing the 802.3 header for this packet.
 * @param header_len    Length of the @p header.
 * @param payload       Packet payload (excluding header).
 * @param payload_len   Length of @p payload.
 * @param arg           Opaque argument that was given when the callback was registered.
 */
typedef void (*mmwlan_rx_cb_t)(uint8_t *header,
                               unsigned header_len,
                               uint8_t *payload,
                               unsigned payload_len,
                               void *arg);

/**
 * Register a receive callback.
 *
 * @note Only one receive callback of any type may be registered. Further registration will
 *       overwrite the previously registered callback (see @ref mmwlan_register_rx_pkt_ext_cb
 *       for exceptions to this).
 *
 * @param callback  The callback to register (@c NULL to unregister).
 * @param arg       Opaque argument to be passed to the callback.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_register_rx_cb(mmwlan_rx_cb_t callback, void *arg);

/**
 * Receive data packet callback function, consuming an mmpkt.
 *
 * @param mmpkt  The mmpkt containing the received packet, including an 802.3 header.
 *               Ownership of the mmpkt is passed to this callback.
 * @param arg    Opaque argument that was given when the callback was registered.
 */
typedef void (*mmwlan_rx_pkt_cb_t)(struct mmpkt *mmpkt, void *arg);

/**
 * Register a receive callback which consumes an mmpkt.
 *
 * @note Only one receive callback of any type may be registered. Further registration will
 *       overwrite the previously registered callback (see @ref mmwlan_register_rx_pkt_ext_cb
 *       for exceptions to this).
 *
 * @param callback  The callback to register (@c NULL to unregister).
 * @param arg       Opaque argument to be passed to the callback.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_register_rx_pkt_cb(mmwlan_rx_pkt_cb_t callback, void *arg);

/** Receive data packet metadata. */
struct mmwlan_rx_metadata
{
    /** The virtual interface that the packet was received on. */
    enum mmwlan_vif vif;

    /** QoS Traffic ID */
    uint8_t tid;

    /** Transmitter Address (TA) of the received packet. */
    const uint8_t *ta;
};

/**
 * Extended receive data packet callback function, consuming an mmpkt.
 *
 * This is functionally the same as @ref mmwlan_rx_pkt_cb_t except for the inclusion of
 * additional @p metadata.
 *
 * @param mmpkt     The mmpkt containing the received packet, including an 802.3 header.
 *                  Ownership of the mmpkt is passed to this callback.
 * @param metadata  Metadata relating to the received packet.
 * @param arg       Opaque argument that was given when the callback was registered.
 */
typedef void (*mmwlan_rx_pkt_ext_cb_t)(struct mmpkt *mmpkt,
                                       const struct mmwlan_rx_metadata *metadata,
                                       void *arg);

/**
 * Register an extended receive callback which may consume an mmpkt.
 *
 * @note Only one receive callback of this type may be registered for each VIF. Further
 *       registration will overwrite the previously registered callback. Registration of
 *       any callback through this function will override any callbacks previously registered
 *       by @ref mmwlan_register_rx_cb() or @ref mmwlan_register_rx_pkt_cb().
 *
 * @param vif       The VIF to register this callback for. If @ref MMWLAN_VIF_UNSPECIFIED then
 *                  it will be registered for all VIFs.
 * @param callback  The callback to register (@c NULL to unregister).
 * @param arg       Opaque argument to be passed to the callback.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_register_rx_pkt_ext_cb(enum mmwlan_vif vif,
                                                 mmwlan_rx_pkt_ext_cb_t callback,
                                                 void *arg);

/**
 * Blocks until the transmit path is ready for transmit.
 *
 * @param timeout_ms    The maximum time to wait, in milliseconds. If zero then this function
 *                      does not block.
 *
 * @return @ref MMWLAN_SUCCESS on success, @ref MMWLAN_TIMED_OUT if the transmit datapath
 *         was not ready within the given timeout, or another error code as appropriate.
 */
enum mmwlan_status mmwlan_tx_wait_until_ready(uint32_t timeout_ms);

/**
 * Allocate an @c mmpkt data structure for transmission with at least enough space for the given
 * payload length.
 *
 * The return mmpkt can be passed to @ref mmwlan_tx_pkt().
 *
 * @param payload_len   Minimum space required for payload.
 * @param tid           The TID that this packet will be transmitted at. This may be used
 *                      by the allocation function to, for example, prioritize allocation of
 *                      certain classes of traffic.
 *
 * @returns the allocated mmpkt on success or @c NULL on failure.
 */
struct mmpkt *mmwlan_alloc_mmpkt_for_tx(uint32_t payload_len, uint8_t tid);

/** Default transmit timeout. Used by @ref mmwlan_tx() and @ref mmwlan_tx_tid().  */
#define MMWLAN_TX_DEFAULT_TIMEOUT_MS (1000)

/** Default QoS Traffic ID (TID) to use for transmit (@c mmwlan_tx()). */
#define MMWLAN_TX_DEFAULT_QOS_TID (0)

/** Maximum Traffic ID (TID) supported for QoS traffic. */
#define MMWLAN_MAX_QOS_TID (7)

/**
 * Metadata for TX packets.
 *
 * This structure should be initialized using @ref MMWLAN_TX_METADATA_INIT for sensible
 * default values, particularly for forward compatibility with new releases that may add
 * new fields to the struct. For example:
 *
 * @code{.c}
 *     enum mmwlan_status status;
 *     struct mmwlan_tx_metadata metadata = MMWLAN_TX_METADATA_INIT;
 *     // HERE: initialize metadata
 *     status = mmwlan_tx_pkt(pkt, &metadata);
 * @endcode
 */
struct mmwlan_tx_metadata
{
    /**
     * Traffic ID (TID) to use. Must be less than or equal to @ref MMWLAN_MAX_QOS_TID.
     * @see MMWLAN_TX_DEFAULT_QOS_TID
     */
    uint8_t tid;

    /**
     * VIF to transmit on. If @ref MMWLAN_VIF_UNSPECIFIED then the transmit function will
     * attempt to infer the VIF.
     *
     * If the specified VIF is not active, or @ref MMWLAN_VIF_UNSPECIFIED is given and a VIF
     * could not be automatically inferred, this will result in transmit failure with
     * error code @ref MMWLAN_VIF_ERROR.
     */
    enum mmwlan_vif vif;

    /** Optional Receiver Address (RA). May be @c NULL, in which case the Receiver Address will
     *  be derived from the Destination Address (DA), if possible. */
    const uint8_t *ra;
};

/**
 * Initializer for @ref mmwlan_tx_metadata.
 */
#define MMWLAN_TX_METADATA_INIT { MMWLAN_TX_DEFAULT_QOS_TID, MMWLAN_VIF_UNSPECIFIED, NULL }

/**
 * Transmit the given packet. The packet must start with an 802.3 header, which will be
 * translated into an 802.11 header by this function.
 *
 * @code
 *
 *    +----------+----------+-----------+------------------+
 *    | DST ADDR | SRC ADDR | ETHERTYPE |   Payload        |
 *    +----------+----------+-----------+------------------+
 *    ^                                 ^                  ^
 *    |--------802.3 MAC Header---------|                  |
 *    |                                                    |
 *    |                                                    |
 *    |                                                    |
 *    |<----------------------len------------------------->|
 *  data
 *
 * @endcode
 *
 * @note The TID should be set in the tx metadata of @p txbuf before invoking this function.
 *
 * @note This function is non-blocking. It will return immediately if the tx path is blocked.
 *       Use the tx flow control callback.
 *
 * @warning The given @p txbuf must be allocated by @ref mmwlan_alloc_mmpkt_for_tx().
 *
 * @param pkt       mmpkt containing the packet to transmit. This will be consumed by this
 *                  function.
 * @param metadata  Extra information relating to the packet transmission. May be @c NULL,
 *                  in which case default values will be used.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_tx_pkt(struct mmpkt *pkt, const struct mmwlan_tx_metadata *metadata);

/**
 * Transmit the given packet using the given QoS Traffic ID (TID). The packet must start with
 * an 802.3 header that will be translated into an 802.11 header.
 *
 * @code
 *
 *    +----------+----------+-----------+------------------+
 *    | DST ADDR | SRC ADDR | ETHERTYPE |   Payload        |
 *    +----------+----------+-----------+------------------+
 *    ^                                 ^                  ^
 *    |--------802.3 MAC Header---------|                  |
 *    |                                                    |
 *    |                                                    |
 *    |                                                    |
 *    |<----------------------len------------------------->|
 *  data
 *
 * @endcode
 *
 * @param data  Packet data.
 * @param len   Length of packet.
 * @param tid   TID to use (0 - @ref MMWLAN_MAX_QOS_TID).
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
static inline enum mmwlan_status mmwlan_tx_tid(const uint8_t *data, unsigned len, uint8_t tid)
{
    enum mmwlan_status status;
    struct mmpkt *pkt;
    struct mmpktview *pktview;
    struct mmwlan_tx_metadata metadata = MMWLAN_TX_METADATA_INIT;

    status = mmwlan_tx_wait_until_ready(MMWLAN_TX_DEFAULT_TIMEOUT_MS);
    if (status != MMWLAN_SUCCESS)
    {
        return status;
    }

    pkt = mmwlan_alloc_mmpkt_for_tx(len, tid);
    if (pkt == NULL)
    {
        return MMWLAN_NO_MEM;
    }

    pktview = mmpkt_open(pkt);
    mmpkt_append_data(pktview, data, len);
    mmpkt_close(&pktview);

    metadata.tid = tid;
    return mmwlan_tx_pkt(pkt, &metadata);
}

/**
 * Transmit the given packet using @ref MMWLAN_TX_DEFAULT_QOS_TID. The packet must start with an
 * 802.3 header that will be translated into an 802.11 header.
 *
 * @code
 *
 *    +----------+----------+-----------+------------------+
 *    | DST ADDR | SRC ADDR | ETHERTYPE |   Payload        |
 *    +----------+----------+-----------+------------------+
 *    ^                                 ^                  ^
 *    |--------802.3 MAC Header---------|                  |
 *    |                                                    |
 *    |                                                    |
 *    |                                                    |
 *    |<----------------------len------------------------->|
 *  data
 *
 * @endcode
 *
 * @param data  Packet data.
 * @param len   Length of packet.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
static inline enum mmwlan_status mmwlan_tx(const uint8_t *data, unsigned len)
{
    return mmwlan_tx_tid(data, len, MMWLAN_TX_DEFAULT_QOS_TID);
}

/**
 * Enumeration of states that can be returned by the transmit flow control callback (as
 * registered by @ref mmwlan_register_tx_flow_control_cb().
 */
enum mmwlan_tx_flow_control_state
{
    MMWLAN_TX_READY, /**< Transmit data path ready for packets (not paused). */
    MMWLAN_TX_PAUSED, /**< Transmit data path paused (blocked). */
};

/**
 * Transmit flow control callback function type. When registered, this callback will be
 * invoked when the transmit data path is paused and when unpaused.
 *
 * @note This function will always be invoked from the Upper MAC thread context. Therefore its
 *       invocation may not be synchronous with changes in flow control state.
 *
 * @param state         Current transmit flow control state.
 * @param arg           Opaque argument that was given when the function was registered.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
typedef void (*mmwlan_tx_flow_control_cb_t)(enum mmwlan_tx_flow_control_state state, void *arg);

/**
 * Register a transmit flow control callback. This callback will be invoked when the tx data path
 * is paused and when unpaused.
 *
 * @note This function will always be invoked from the Upper MAC thread context. Therefore its
 *       invocation may not be synchronous with changes in flow control state.
 *
 * @param cb    The callback to register.
 * @param arg   Opaque argument to pass to the callback.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_register_tx_flow_control_cb(mmwlan_tx_flow_control_cb_t cb, void *arg);

/** @} */

/*
 * ---------------------------------------------------------------------------------------------
 */

/**
 * @defgroup MMWLAN_STATS    WLAN Statistics API
 *
 * API for retrieving statistics information from the WLAN subsystem.
 *
 * @{
 */

/** Rate control statistics data structure. */
struct mmwlan_rc_stats
{
    /** The number of rate table entries. */
    uint32_t n_entries;
    /** Rate info for each rate table entry. */
    uint32_t *rate_info;
    /** Total number of packets sent for each rate table entry. */
    uint32_t *total_sent;
    /** Total successes for each rate table entry. */
    uint32_t *total_success;
};

/**
 * Enumeration defined offsets into the bit field of rate information (@c rate_info in
 * @ref mmwlan_rc_stats).
 *
 * ```
 * 31         9       8      4    0
 * +----------+-------+------+----+
 * | Reserved | Guard | Rate | BW |
 * |    23    |   1   |  4   | 4  |
 * +----------+-------+------+----+
 * ```
 * * BW: 0 = 1 MHz, 1 = 2 MHz, 2 = 4 MHz
 * * Rate: MCS Rate
 * * Guard: 0 = LGI, 1 = SGI
 */
enum mmwlan_rc_stats_rate_info_offsets
{
    MMWLAN_RC_STATS_RATE_INFO_BW_OFFSET = 0,
    MMWLAN_RC_STATS_RATE_INFO_RATE_OFFSET = 4,
    MMWLAN_RC_STATS_RATE_INFO_GUARD_OFFSET = 8,
};

/**
 * Retrieves WLAN rate control statistics.
 *
 * @note The returned data structure must be freed using @ref mmwlan_free_rc_stats().
 *
 * @returns the statistics data structure on success or @c NULL on failure.
 */
struct mmwlan_rc_stats *mmwlan_get_rc_stats(void);

/**
 * Free a mmwlan_rc_stats structure that was allocated with @ref mmwlan_get_rc_stats().
 *
 * @param stats     The structure to be freed (may be @c NULL).
 */
void mmwlan_free_rc_stats(struct mmwlan_rc_stats *stats);

/**
 * Data structure used to represent an opaque buffer containing Morse statistics. This is
 * returned by @c mmwlan_get_morse_stats() and must be freed by @c mmwlan_free_morse_stats().
 */
struct mmwlan_morse_stats
{
    /** Buffer containing the stats. */
    uint8_t *buf;
    /** Length of stats in @c buf. */
    uint32_t len;
};

/**
 * Retrieves statistics from the Morse transceiver. The stats are returned as a binary blob
 * that can be parsed by host tools.
 *
 * @param core_num  The core to retrieve stats for.
 * @param reset     Boolean indicating whether to reset the stats after retrieving.
 *
 * @note The returned @c mmwlan_morse_stats instance must be freed using
 *       @ref mmwlan_free_morse_stats.
 *
 * @returns a @c mmwlan_morse_stats instance on success or @c NULL on failure.
 */
struct mmwlan_morse_stats *mmwlan_get_morse_stats(uint32_t core_num, bool reset);

/**
 * Frees a @c mmwlan_morse_stats instance that was returned by @c mmwlan_get_morse_stats().
 *
 * @param stats     The instance to free. May be @ NULL.
 */
void mmwlan_free_morse_stats(struct mmwlan_morse_stats *stats);

/** @ingroup MMWLAN_UMAC_STATS */

/** @{ */

/**
 * The data for this struct is auto-generated, so it is stored externally.
 * See mmwlan_stats.h for definition.
 */
struct mmwlan_stats_umac_data;

/**
 * Gets the current values of the UMAC statistics.
 *
 * @param stats_dest An @c mmwlan_umac_stats pointer where the data will be stored.
 *
 * @return @ref MMWLAN_SUCCESS if stats retrieved or @ref MMWLAN_INVALID_ARGUMENT
 *          if the buffer is NULL.
 */
enum mmwlan_status mmwlan_get_umac_stats(struct mmwlan_stats_umac_data *stats_dest);

/**
 * Clear all current values of the UMAC statistics.
 *
 * @return @ref MMWLAN_SUCCESS
 */
enum mmwlan_status mmwlan_clear_umac_stats(void);

/** @} */

/** @} */

/*
 * ---------------------------------------------------------------------------------------------
 */

/**
 * @defgroup MMWLAN_TEST    WLAN Test (ATE) API
 *
 * Extended API particularly intended for test use cases.
 *
 * @{
 */

/** Enumeration of MCS rates. */
enum mmwlan_mcs
{
    MMWLAN_MCS_NONE = -1, /**< Use-case specific special value */
    MMWLAN_MCS_0 = 0, /**< MCS0 */
    MMWLAN_MCS_1, /**< MCS1 */
    MMWLAN_MCS_2, /**< MCS2 */
    MMWLAN_MCS_3, /**< MCS3*/
    MMWLAN_MCS_4, /**< MCS4 */
    MMWLAN_MCS_5, /**< MCS5 */
    MMWLAN_MCS_6, /**< MCS6 */
    MMWLAN_MCS_7, /**< MCS7 */
    MMWLAN_MCS_8, /**< MCS8 */
    MMWLAN_MCS_9, /**< MCS9 */
    MMWLAN_MCS_MAX = MMWLAN_MCS_9 /**< Maximum supported MCS rate */
};

/** Enumeration of bandwidths. */
enum mmwlan_bw
{
    MMWLAN_BW_NONE = -1, /**< Use-case specific special value */
    MMWLAN_BW_1MHZ = 1, /**< 1 MHz bandwidth */
    MMWLAN_BW_2MHZ = 2, /**< 2 MHz bandwidth */
    MMWLAN_BW_4MHZ = 4, /**< 4 MHz bandwidth */
    MMWLAN_BW_8MHZ = 8, /**< 8 MHz bandwidth */
    MMWLAN_BW_MAX = MMWLAN_BW_8MHZ, /**< Maximum supported bandwidth */
};

/** Enumeration of guard intervals. */
enum mmwlan_gi
{
    MMWLAN_GI_NONE = -1, /**< Use-case specific special value */
    MMWLAN_GI_SHORT = 0, /**< Short guard interval */
    MMWLAN_GI_LONG, /**< Long guard interval */
    MMWLAN_GI_MAX = MMWLAN_GI_LONG, /**< Maximum valid value of this @c enum. */
};

/**
 * Enable/disable override of rate control parameters.
 *
 * @param tx_rate_override      Overrides the transmit MCS rate. Set to @ref MMWLAN_MCS_NONE for no
 *                              override.
 * @param bandwidth_override    Overrides the TX bandwidth. Set to @ref MMWLAN_BW_NONE for no
 *                              override.
 * @param gi_override           Overrides the guard interval. Set to @ref MMWLAN_GI_NONE for no
 *                              override.
 *
 * @return @ref MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_ate_override_rate_control(enum mmwlan_mcs tx_rate_override,
                                                    enum mmwlan_bw bandwidth_override,
                                                    enum mmwlan_gi gi_override);

/**
 * Execute a test/debug command. The format of command and response is opaque to this API.
 *
 * @param[in]     command       Buffer containing the command to be executed. Note that buffer
 *                              contents may be modified by this function.
 * @param[in]     command_len   Length of the command to be executed.
 * @param[out]    response      Buffer to received the response to the command.
 *                              May be NULL, in which case @p response_len should also be NULL.
 * @param[in,out] response_len  Pointer to a @c uint32_t that is initialized to the length of the
 *                              response buffer. On success, the value will be updated to the
 *                              length of data that was put into the response buffer.
 *                              May be NULL, in which case @p response must also be NULL.
 *
 * @returns @c MMWLAN_SUCCESS on success, else an appropriate error code.
 */
enum mmwlan_status mmwlan_ate_execute_command(uint8_t *command,
                                              uint32_t command_len,
                                              uint8_t *response,
                                              uint32_t *response_len);

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
