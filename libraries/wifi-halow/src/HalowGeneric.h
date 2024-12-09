/*
 HalowGeneric.h - esp32 HaLow support.
 Based on HaLow.h from Ardiono HaLow shield library.
 Copyright (c) 2011-2014 Arduino.  All right reserved.
 Modified by Ivan Grokhotkov, December 2014
 Reworked by Markus Sattler, December 2015
 Additions Copyright (c) 2024 Heltec AutoMation.
 
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

#ifndef ESP32HALOWGENERIC_H_
#define ESP32HALOWGENERIC_H_

#include "esp_err.h"
#include "esp_event.h"
#include <functional>
#include "IPAddress.h"
#include "esp_smartconfig.h"
#include "esp_netif_types.h"
#include "esp_eth_driver.h"
#include "lwip/ip_addr.h"
#include "mmwlan.h"
#include "mmipal.h"
#include "mmosal.h"
#include "loadconfig.h"
#include "mmwlan_regdb.h"
#include "WiFiType.h"

#include "HardwareSerial.h"
#include <string.h>
#include "halow_config.h"


ESP_EVENT_DECLARE_BASE(ARDUINO_HALOW_EVENTS);
ESP_EVENT_DECLARE_BASE(HALOW_EVENTS);

typedef enum {
	HALOW_EVENT_READY = 0,
	HALOW_EVENT_SCAN_DONE,
	HALOW_EVENT_STA_START,
	HALOW_EVENT_STA_STOP,
	HALOW_EVENT_STA_CONNECTING,
	HALOW_EVENT_STA_CONNECTED,
	HALOW_EVENT_STA_DISCONNECTED,
	HALOW_EVENT_STA_GOT_IP,
	HALOW_EVENT_STA_LOST_IP
} wifi_halow_event_t;

typedef enum {
	ARDUINO_HALOW_EVENT_READY = 0,
	ARDUINO_HALOW_EVENT_SCAN_DONE,
	ARDUINO_HALOW_EVENT_STA_START,
	ARDUINO_HALOW_EVENT_STA_STOP,
	ARDUINO_HALOW_EVENT_STA_CONNECTING,
	ARDUINO_HALOW_EVENT_STA_CONNECTED,
	ARDUINO_HALOW_EVENT_STA_DISCONNECTED,
	ARDUINO_HALOW_EVENT_STA_GOT_IP,
	ARDUINO_HALOW_EVENT_STA_LOST_IP,
	ARDUINO_HALOW_EVENT_MAX,
} arduino_halow_event_id_t;


#define HaLowEvent_t  arduino_halow_event_id_t

typedef struct{
	arduino_halow_event_id_t event_id;
} arduino_halow_event_t;

typedef void (*HalowEventCb)(arduino_halow_event_id_t event);

typedef size_t halow_event_id_t;

// General Flags
static const int HALOW_NET_DNS_IDLE_BIT       = BIT0;
static const int HALOW_NET_DNS_DONE_BIT       = BIT1;
// WiFi Scan Flags
static const int HALOW_SCANNING_BIT      = BIT2;
static const int HALOW_SCAN_DONE_BIT     = BIT3;
// STA Flags
static const int HALOW_STA_STARTED_BIT        = BIT4;
static const int HALOW_STA_CONNECTED_BIT      = BIT5;
static const int HALOW_STA_HAS_IP_BIT         = BIT6;
static const int HALOW_STA_HAS_IP6_BIT        = BIT7;
static const int HALOW_STA_HAS_IP6_GLOBAL_BIT = BIT8;
static const int HALOW_STA_WANT_IP6_BIT       = BIT9;
// Masks
static const int HALOW_NET_HAS_IP6_GLOBAL_BIT = HALOW_STA_HAS_IP6_GLOBAL_BIT ;



class HalowGenericClass
{
  public:
    HalowGenericClass();
    
    halow_event_id_t onEvent(HalowEventCb cbEvent, halow_event_id_t event = ARDUINO_HALOW_EVENT_MAX);
    void removeEvent(HalowEventCb cbEvent, halow_event_id_t event = ARDUINO_HALOW_EVENT_MAX);

    static int getStatusBits();
    static int waitStatusBits(int bits, uint32_t timeout_ms);

    int32_t channel(void);

    void persistent(bool persistent);
    void enableLongRange(bool enable);

    const char * eventName(halow_event_id_t id);

    static esp_err_t _eventCallback(arduino_halow_event_t *event);
    
    static void useStaticBuffers(bool bufferMode);
    static bool useStaticBuffers();

    static int hostByName(const char *aHostname, IPAddress &aResult, bool preferV6=false);

    static IPAddress calculateNetworkID(IPAddress ip, IPAddress subnet);
    static IPAddress calculateBroadcast(IPAddress ip, IPAddress subnet);
    static uint8_t calculateSubnetCIDR(IPAddress subnetMask);

    static int setStatusBits(int bits);
    static int clearStatusBits(int bits);
  protected:

    friend class HalowSTAClass;

};
uint64_t getID();
extern bool halowtcpipInit();

#endif /* ESP32HalowGeneric_H_ */
