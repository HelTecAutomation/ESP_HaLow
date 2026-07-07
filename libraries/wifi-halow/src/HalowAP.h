/*
 HaLowAP.h - HaLow AP mode library for esp32

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

#ifndef ESP32HALOWAP_H_
#define ESP32HALOWAP_H_

#include "mmwlan.h"
#include "mmnetif.h"
#include "mmhal.h"

#include "WiFiType.h"
#include "HalowGeneric.h"
#ifdef ESP_IDF_VERSION_MAJOR
#include "esp_event.h"
#endif


class HalowAPClass
{
public:
    enum mmwlan_status enable(void);
    enum mmwlan_status disable(void);

    bool isEnabled();

    IPAddress softAPIP();
    uint8_t *BSSID(uint8_t *bssid);
    String BSSIDstr();

    // Connected STA info
    uint8_t getStationList(struct mmwlan_ap_sta_status *sta_list, uint8_t max_count);
    uint8_t getMaxStas() { return ap_args.max_stas; }
    void setMaxStas(uint8_t max_stas) { ap_args.max_stas = max_stas; }

    String SSID() const;
    uint16_t getChannel() const { return ap_args.s1g_chan_num; }
    uint16_t getOpClass() const { return ap_args.op_class; }

protected:
    static void handleStaStatus(const struct mmwlan_ap_sta_status *sta_status, void *arg);
    struct mmwlan_ap_args ap_args = MMWLAN_AP_ARGS_INIT;
    bool _enabled = false;
};
#endif
