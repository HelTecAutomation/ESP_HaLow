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
extern struct netif lwip_mmnetif;

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
