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

/* Must be defined before Arduino.h/IPAddress.h pull in lwip/opt.h, otherwise
 * the struct netif layout differs from the precompiled lwIP/esp_netif libs. */
#ifndef LWIP_NETIF_LINK_CALLBACK
#define LWIP_NETIF_LINK_CALLBACK 1
#endif

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
#include "hostap_morse_common.h"
#include "mmregdb.h"

#define HALOW_AP_MAX_STAS 8
#define HALOW_MESH_MAX_PEERS 8

class HalowAPClass
{
public:
    enum mmwlan_status enable(void);
    enum mmwlan_status disable(void);

    bool isEnabled();

    // DHCP server. Call enableDhcpServer() after AP() has started.
    // The lease callback runs in the lwIP thread; keep it short and non-blocking.
    typedef void (*DhcpLeaseCb)(IPAddress client_ip, const uint8_t client_mac[6]);
    bool enableDhcpServer(IPAddress lease_start, IPAddress lease_end,
                          IPAddress dns = IPAddress());
    bool enableDhcpServer();
    bool disableDhcpServer();
    bool isDhcpServerEnabled();
    void setDhcpLeaseCallback(DhcpLeaseCb cb);

    IPAddress softAPIP();
    uint8_t *BSSID(uint8_t *bssid);
    String BSSIDstr();

    // Connected STA info
    uint8_t getStationList(struct mmwlan_ap_sta_status *sta_list, uint8_t max_count);
    uint8_t getMaxStas() { return ap_args.max_stas; }
    void setMaxStas(uint8_t max_stas)
    {
        ap_args.max_stas = max_stas > HALOW_AP_MAX_STAS ? HALOW_AP_MAX_STAS : max_stas;
    }

    String SSID() const;
    uint16_t getChannel() const { return ap_args.s1g_chan_num; }
    uint16_t getOpClass() const { return ap_args.op_class; }
    uint8_t getBandwidth() const;
    uint32_t getFrequency() const;

protected:
    static void handleStaStatus(const struct mmwlan_ap_sta_status *sta_status, void *arg);
    struct mmwlan_ap_args ap_args = MMWLAN_AP_ARGS_INIT;
    bool _enabled = false;
};

class HalowClass :public HalowSTAClass, public HalowGenericClass, public HalowScanClass, public HalowAPClass
{
public:
	void init(const char *region = "US");
	bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet,IPAddress dns1 = (uint32_t)0x00000000, IPAddress dns2 = (uint32_t)0x00000000);
	bool AP(const char *ssid, const char *passphrase = NULL,uint16_t channel = 0,uint8_t bandwidth_mhz=0,int max_connection=4);

	/*
	 * Start 802.11s mesh mode (the mesh ID is used as the SSID).
	 * Call config() before meshBegin() to use a static IP on every node;
	 * otherwise the existing DHCP configuration is used.
	 * The region is applied only if the radio has not been initialized yet.
	 */
	bool meshBegin(const char *mesh_id, const char *passphrase = NULL,
	               uint16_t channel = 0, uint8_t bandwidth_mhz = 0,
	               const char *region = "US");
	bool isMeshMode() const { return _mesh_mode; }
	bool meshSetPeerWhitelist(const uint8_t (*peers)[MMWLAN_MAC_ADDR_LEN], size_t count);
	bool meshGetStats(struct mmwlan_mesh_fwd_stats *stats);
	bool meshClearStats(void);
	size_t meshGetLinkStats(struct mmwlan_mesh_link_stats *stats, size_t max);
	void meshClearLinkStats(void);
	void meshTriggerPreq(const uint8_t dest[MMWLAN_MAC_ADDR_LEN]);
	
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
	bool _ip6_disabled=false;
	char _region[16] = {0};
};

extern HalowClass HaLow;
extern "C" void ip_napt_enable(u32_t addr, int enable);
#endif
