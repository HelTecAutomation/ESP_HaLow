/*
 * Copyright 2021-2023 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "lwip/netif.h"
#include "lwip/err.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct mmipal_data
{
    struct netif lwip_mmnetif;
    /** This stores the IPv4 link state for the IP stack. I.e., do we have an IP address or not. */
    enum mmipal_link_state ip_link_state;
    /** Flag requesting ARP response offload feature */
    bool offload_arp_response;
    /** ARP refresh offload interval in seconds */
    uint32_t offload_arp_refresh_s;
    /** Initial dhcp offload call has been completed */
    bool dhcp_offload_init_complete;
    /** The link status callback function that has been registered. */
    mmipal_link_status_cb_fn_t link_status_callback;
    /** The extended link status callback function that has been registered. */
    mmipal_ext_link_status_cb_fn_t ext_link_status_callback;
    /** Argument for the extended link status callback function that has been registered. */
    void *ext_link_status_callback_arg;
#if LWIP_IPV4
    enum mmipal_addr_mode ip4_mode;
#endif
#if LWIP_IPV6
    enum mmipal_ip6_addr_mode ip6_mode;
#endif
};

extern struct mmipal_data mmipal_data;

/** Initializer for the Morse Micro network interface */
err_t mmnetif_init(struct netif *netif);

/**
 * Configure the QoS TID for the @c netif. QoS data will be sent using this TID.
 *
 * @param netif The @c netif to configure.
 * @param tid   The TID value to set.
 */
void mmnetif_set_tx_qos_tid(struct netif *netif, uint8_t tid);

#ifdef __cplusplus
}
#endif
