/*
 HaLowSTA.h - HaLowSTA library for esp32

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

#ifndef ESP32HALOWSTA_H_
#define ESP32HALOWSTA_H_

#include "mmwlan.h"
#include "mmnetif.h"
#include "mmhal.h"

#include "WiFiType.h"
#include "HalowGeneric.h"
#ifdef ESP_IDF_VERSION_MAJOR
#include "esp_event.h"
#endif


class HalowSTAClass
{
    // ----------------------------------------------------------------------------------------------
    // ---------------------------------------- STA function ----------------------------------------
    // ----------------------------------------------------------------------------------------------

public:

    wl_status_t begin(const char* ssid, const char *passphrase, mmwlan_security_type_t security_type = MMWLAN_SAE,const char *region = "US");

	bool setDNS(IPAddress dns1, IPAddress dns2 = (uint32_t)0x00000000);  // sets DNS IP for all network interfaces

    bool isConnected();	

    // STA network info
    IPAddress localIP();

    uint8_t * macAddress(uint8_t* mac);
    String macAddress();

    IPAddress subnetMask();
    IPAddress gatewayIP();	
    IPAddress dnsIP(uint8_t dns_no = 0);

    IPAddress broadcastIP();
    IPAddress networkID();
    uint8_t subnetCIDR();

    // STA Halow info
    static wl_status_t status();
    String SSID() const;
    String psk() const;

    uint8_t * BSSID(uint8_t* bssid = NULL);
    String BSSIDstr();

    int8_t RSSI();

    static void _setStatus(wl_status_t status);
private:
	struct mmwlan_sta_args sta_args = MMWLAN_STA_ARGS_INIT;
};

//extern HalowSTAClass HaLow;

#endif /* ESP32HalowSTA_H_ */
