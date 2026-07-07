/*
 Halow.h - esp32s3 Halow support.
 Based on Halow.h from Arduino Halow shield library.
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

#ifndef Halow_h
#define Halow_h

#include <stdint.h>

#include "Print.h"
#include "IPAddress.h"

//#include "HalowScan.h"
#include "HalowGeneric.h"
#include "HalowSTA.h"
#include "HalowScan.h"

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
#include "HalowClient.h"
#include "WiFiServer.h"
#include "HalowAP.h"
#include "hostap_morse_common.h"
#include "mmwlan_regdb.h"
class HalowClass :public HalowSTAClass, public HalowGenericClass, public HalowScanClass, public HalowAPClass
{
public:
	void init(const char *region = "US");
	bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet,IPAddress dns1 = (uint32_t)0x00000000, IPAddress dns2 = (uint32_t)0x00000000);
	bool AP(const char *ssid, const char *passphrase = NULL,uint16_t channel = 0,int max_connection=4);
	
	using HalowSTAClass::SSID;
	using HalowSTAClass::RSSI;
	using HalowSTAClass::BSSID;
	using HalowSTAClass::BSSIDstr;
    using HalowSTAClass::setDNS;
	using HalowScanClass::SSID;
	using HalowScanClass::encryptionType;
	using HalowScanClass::RSSI;
	using HalowScanClass::BSSID;
	using HalowScanClass::BSSIDstr;
	using HalowScanClass::channel;

	friend class HalowClient;
	friend class NetworkServer;

private:
	struct mmipal_init_args mmipal_init_args = MMIPAL_INIT_ARGS_DEFAULT;
	const struct mmwlan_s1g_channel_list *channel_list;
	bool _halow_started=false;
	bool _static=false;
};

extern HalowClass HaLow;
extern "C" void ip_napt_enable(u32_t addr, int enable);
#endif
