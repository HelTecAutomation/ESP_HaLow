/*
 * Copyright 2021-2023 Morse Micro
 *
 * This file is licensed under terms that can be found in the LICENSE.md file in the root
 * directory of the Morse Micro IoT SDK software package.
 */

#pragma once
#include "halow_config.h"
#include "lwip/netif.h"
#include "lwip/err.h"
#include "halow_config.h"

#ifdef __cplusplus
extern "C" {
#endif
//extern struct netif lwip_mmnetif;

struct mmipal_data
{
	struct netif lwip_mmnetif;
	/** This stores the IPv4 link state for the IP stack. I.e., do we have an IP address or not. */
	enum mmipal_link_state ip_link_state;
	/** Flag requesting ARP response offload feature */
	bool offload_arp_response;
	/** ARP refresh offload interval in seconds */
	uint32_t offload_arp_refresh_s;
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

/** mmnetif initaliser */
err_t mmnetif_init(struct netif *netif);

/**
 * Configure the QoS TID for the netif. QoS data will be sent using this TID.
 *
 * @param netif The netif to configure.
 * @param tid   The TID value to set.
 */
void mmnetif_set_tx_qos_tid(struct netif *netif, uint8_t tid);

#ifdef __cplusplus
}
#endif
