/*
 HaLowScan.h - HaLowScan library for esp32

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

#ifndef ESP32HalowSCAN_H_
#define ESP32HalowSCAN_H_

#include "HalowGeneric.h"




class HalowScanClass
{

public:

    int16_t scanNetworks(bool async = false, bool show_hidden = false, bool passive = false, uint32_t max_ms_per_chan = 300, uint8_t channel = 0, const char * ssid=nullptr, const uint8_t * bssid=nullptr);

    int16_t scanComplete();
    void scanDelete();

    String SSID(uint8_t networkItem);
    mmwlan_security_type_t encryptionType(uint8_t networkItem);
	String encryptionTypeStr(uint8_t i);

    int32_t RSSI(uint8_t networkItem);
    uint8_t * BSSID(uint8_t networkItem, uint8_t* bssid = NULL);
    String BSSIDstr(uint8_t networkItem);
    int32_t channel(uint8_t networkItem);
    static void * getScanInfoByIndex(int i) { return _getScanInfoByIndex(i); }; 

    static void _scanDone();
protected:

    static bool _scanAsync;
    
    static uint32_t _scanStarted;
    static uint32_t _scanTimeout;
    static uint16_t _scanCount;
    
    static void* _scanResult;

    static void * _getScanInfoByIndex(int i);

};


#endif /* ESP32HalowSCAN_H_ */
