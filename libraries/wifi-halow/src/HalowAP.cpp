/*
 HaLowAP.cpp - HaLow AP mode library for esp32

 Copyright (c) 2024 Heltec AutoMation.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "HalowGeneric.h"
#include "HalowAP.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <esp_err.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp32-hal.h>
#include <lwip/ip_addr.h>
#include "lwip/err.h"
#include "lwip/dns.h"
#include <esp_smartconfig.h>
#include <esp_netif.h>
#include "esp_mac.h"
#include "halow_config.h"
#include "mmwlan.h"
#include "mmhal.h"
#include "mmosal.h"
#include "mmipal.h"
#include "WiFiType.h"
#include "HaLow.h"


void HalowAPClass::handleStaStatus(const struct mmwlan_ap_sta_status *sta_status, void *arg)
{
    (void)sta_status;
    (void)arg;
}

enum mmwlan_status HalowAPClass::enable(void)
{
    enum mmwlan_status status = mmwlan_ap_enable(&ap_args);
    if (status == MMWLAN_SUCCESS)
    {
        _enabled = true;
    }
    return status;
}

enum mmwlan_status HalowAPClass::disable(void)
{
    enum mmwlan_status status = mmwlan_ap_disable();
    if (status == MMWLAN_SUCCESS)
    {
        _enabled = false;
    }
    return status;
}

bool HalowAPClass::isEnabled()
{
    return _enabled;
}

String HalowAPClass::SSID() const
{
    return String((char *)ap_args.ssid);
}


uint8_t *HalowAPClass::BSSID(uint8_t *bssid)
{
    enum mmwlan_status err = mmwlan_ap_get_bssid(bssid);
    if (err != MMWLAN_SUCCESS)
        memset(bssid, 0, 6);
    return bssid;
}

String HalowAPClass::BSSIDstr(void)
{
    uint8_t bssid[6] = { 0 };
    if (mmwlan_ap_get_bssid(bssid) != MMWLAN_SUCCESS)
        return String();
    char mac[18] = { 0 };
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
            bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    return String(mac);
}

uint8_t HalowAPClass::getStationList(struct mmwlan_ap_sta_status *sta_list, uint8_t max_count)
{
    uint8_t count = 0;
    return count;
}

bool HalowClass::AP(const char *ssid, const char *passphrase, uint16_t channel,int max_connection)
{
    if (!_halow_started)
        init();
    halow_mode=MMWLAN_VIF_AP;
    (void)mmosal_safer_strcpy((char *)ap_args.ssid, ssid, sizeof(ap_args.ssid));
    ap_args.ssid_len = strlen((char *)ap_args.ssid);

    if (passphrase != NULL)
    {
        (void)mmosal_safer_strcpy(ap_args.passphrase, passphrase, sizeof(ap_args.passphrase));
        ap_args.passphrase_len = strlen(ap_args.passphrase);
    }
    else
    {
        ap_args.passphrase[0] = '\0';
        ap_args.passphrase_len = 0;
    }

    enum mmwlan_security_type security = MMWLAN_SAE;
    if (passphrase == NULL || passphrase[0] == '\0')
        security = MMWLAN_OPEN;

    ap_args.security_type = security;
    if (channel == 0)
        channel = halow_channel_list->channels[halow_channel_list->num_channels - 1].s1g_chan_num;

    uint16_t op_class = 0;
    uint8_t  ap_bw=8;
    for (unsigned i = 0; i < halow_channel_list->num_channels; i++)
    {
        if (halow_channel_list->channels[i].s1g_chan_num == channel)
        {
            op_class = halow_channel_list->channels[i].global_operating_class;
            ap_bw = halow_channel_list->channels[i].bw_mhz;
            break;
        }
    }
    if (op_class == 0)
    {
        printf("Error: channel %d not found, fallback to channel %d\n",
            channel, halow_channel_list->channels[halow_channel_list->num_channels - 1].s1g_chan_num);
        channel = halow_channel_list->channels[halow_channel_list->num_channels - 1].s1g_chan_num;
        op_class = halow_channel_list->channels[halow_channel_list->num_channels - 1].global_operating_class;
        ap_bw = halow_channel_list->channels[halow_channel_list->num_channels - 1].bw_mhz;
    }

    ap_args.op_class = op_class;
    ap_args.s1g_chan_num = channel;
    ap_args.sta_status_cb = HalowAPClass::handleStaStatus;
    ap_args.sta_status_cb_arg = NULL;
    ap_args.max_stas = max_connection;
    ap_args.pmf_mode = MMWLAN_PMF_REQUIRED;
    switch(ap_bw)
    {
        case 1:
        case 2:
            ap_args.pri_bw_mhz = 0;
            ap_args.pri_1mhz_chan_idx = 0;
            break;
        case 4:
            ap_args.pri_bw_mhz = 1;
            ap_args.pri_1mhz_chan_idx = 2;
            break;
        case 8:
            ap_args.pri_bw_mhz = 1;
            ap_args.pri_1mhz_chan_idx = 3;
            break;
        default:
            ap_args.pri_bw_mhz = 0;
            ap_args.pri_1mhz_chan_idx = 0;
            break;
    }
    //printf("channel %d, op_class %d\r\n",ap_args.s1g_chan_num,ap_args.op_class);
    //printf("pri_bw_mhz %d, pri_1mhz_chan_idx %d\r\n",ap_args.pri_bw_mhz,ap_args.pri_1mhz_chan_idx);
    mmwlan_set_power_save_mode(MMWLAN_PS_DISABLED);
    enum mmwlan_status status = enable();
    return status == MMWLAN_SUCCESS;
}
