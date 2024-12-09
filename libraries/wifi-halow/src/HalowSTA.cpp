/*
 HaLowSTA.cpp - HaLowSTA library for esp32

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


//#include "WiFi.h"
#include "HalowGeneric.h"
#include "HalowSTA.h"

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
#include "WiFiType.h"



// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------- STA function -----------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------



/** Maximum number of DNS servers to attempt to retrieve from config store. */
#ifndef DNS_MAX_SERVERS
#define DNS_MAX_SERVERS                 2
#endif


/**
 * WLAN station status callback, invoked when WLAN STA state changes.
 *
 * @param sta_state  The new STA state.
 */
static void sta_status_callback(enum mmwlan_sta_state sta_state)
{
    switch (sta_state)
    {
    case MMWLAN_STA_DISABLED:
        //printf("WLAN STA disabled\n");
        esp_event_post(HALOW_EVENTS, HALOW_EVENT_STA_DISCONNECTED, NULL, 0, portMAX_DELAY);
        break;

    case MMWLAN_STA_CONNECTING:
        //printf("WLAN STA connecting\n");
        esp_event_post(HALOW_EVENTS, HALOW_EVENT_STA_CONNECTING, NULL, 0, portMAX_DELAY);
        break;

    case MMWLAN_STA_CONNECTED:
        //printf("WLAN STA connected\n");
        esp_event_post(HALOW_EVENTS, HALOW_EVENT_STA_CONNECTED, NULL, 0, portMAX_DELAY);
        break;
    }
}


void app_wlan_stop(void)
{
    /* Shutdown wlan interface */
    mmwlan_shutdown();
}


static wl_status_t _sta_status = WL_STOPPED;
static EventGroupHandle_t _sta_status_group = NULL;

void HalowSTAClass::_setStatus(wl_status_t status)
{
    if(!_sta_status_group){
        _sta_status_group = xEventGroupCreate();
        if(!_sta_status_group){
            log_e("STA Status Group Create Failed!");
            _sta_status = status;
            return;
        }
    }
    xEventGroupClearBits(_sta_status_group, 0x00FFFFFF);
    xEventGroupSetBits(_sta_status_group, status);
}

/**
 * Return Connection status.
 * @return one of the value defined in wl_status_t
 *
 */
wl_status_t HalowSTAClass::status()
{
   if(!_sta_status_group){
	   return _sta_status;
   }
   return (wl_status_t)xEventGroupClearBits(_sta_status_group, 0);

}



/**
 * Start Wifi connection
 * if passphrase is set the most secure supported mode will be automatically selected
 * @param ssid const char*          Pointer to the SSID string.
 * @param passphrase const char *   Optional. Passphrase. Valid characters in a passphrase must be between ASCII 32-126 (decimal).
 * @param channel                   Optional. Channel of AP
 * @return
 */
wl_status_t HalowSTAClass::begin(const char* ssid, const char *passphrase, mmwlan_security_type_t security_type,const char *region)
{
	enum mmwlan_status _status;
	struct mmwlan_version version;

/*
	halowtcpipInit();

	enum mmwlan_status _status;
	struct mmwlan_version version;

	MMOSAL_ASSERT(link_established == NULL);
	link_established = mmosal_semb_create("link_established");


	mmhal_init();
	mmwlan_init();

	char strval[16];
	(void)mmosal_safer_strcpy(strval, region, sizeof(strval));
	channel_list = mmwlan_lookup_regulatory_domain(get_regulatory_db(), strval);
	if (channel_list == NULL)
	{
		printf("Could not find specified regulatory domain matching country code %s\n", strval);
		printf("Please set the configuration key wlan.country_code to the correct country code.\n");
		MMOSAL_ASSERT(false);
	}

	mmwlan_set_channel_list(channel_list);

*/
	/* Load IP stack settings from config store, or use defaults if no entry found in
	 * config store. */

	//load_mmipal_init_args(&mmipal_init_args);






//	_status = mmwlan_get_version(&version);
//	MMOSAL_ASSERT(_status == MMWLAN_SUCCESS);
//	printf("Morse firmware version %s, morselib version %s, Morse chip ID 0x%lx\n\n",
//		   version.morse_fw_version, version.morselib_version, version.morse_chip_id);

    /* Load SSID */
    (void)mmosal_safer_strcpy((char*)sta_args.ssid, ssid, sizeof(sta_args.ssid));
    sta_args.ssid_len = strlen((char*)sta_args.ssid);

    /* Load password */
    (void)mmosal_safer_strcpy(sta_args.passphrase, passphrase, sizeof(sta_args.passphrase));
    sta_args.passphrase_len = strlen(sta_args.passphrase);

    /* Load security type */
    sta_args.security_type = security_type;

    /*
    printf("Attempting to connect to %s ", sta_args.ssid);
    if (sta_args.security_type == MMWLAN_SAE)
    {
        printf("with passphrase %s", sta_args.passphrase);
    }
    printf("\n");
    printf("This may take some time (~30 seconds)\n");
    */

    _status = mmwlan_sta_enable(&sta_args, sta_status_callback);
    MMOSAL_ASSERT(_status == MMWLAN_SUCCESS);
    

    /* Wait for link status callback.
    * Use a binary semaphore to block us until Link is up.
    */
    //mmosal_semb_wait(link_established, UINT32_MAX);

    return status();
}





/**
 * Change DNS server for static IP configuration
 * @param dns1       Static DNS server 1
 * @param dns2       Static DNS server 2 (optional)
 */
bool HalowSTAClass::setDNS(IPAddress dns1, IPAddress dns2)
{
	ip_addr_t dns;
	dns1.to_ip_addr_t(&dns);
	dns_setserver(0, &dns);
	dns2.to_ip_addr_t(&dns);
	dns_setserver(1, &dns);
    return 1;
}

/**
 * Get the DNS ip address.
 * @param dns_no
 * @return IPAddress DNS Server IP
 */
IPAddress HalowSTAClass::dnsIP(uint8_t dns_no)
{
	if (status() != WL_CONNECTED)
	  return IPAddress();
    const ip_addr_t * dns_ip = dns_getserver(dns_no);
    return IPAddress(dns_ip->u_addr.ip4.addr);
}

/**
 * is STA interface connected?
 * @return true if STA is connected to an AP
 */
bool HalowSTAClass::isConnected()
{
    return (status() == WL_CONNECTED);
}

/**
 * Get the station interface IP address.
 * @return IPAddress station IP
 */
IPAddress HalowSTAClass::localIP()
{
  if (status() != WL_CONNECTED)
    return IPAddress();
  return IPAddress(lwip_mmnetif.ip_addr.u_addr.ip4.addr);
}


/**
 * Get the station interface MAC address.
 * @param mac   pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
 * @return      pointer to uint8_t *
 */
uint8_t* HalowSTAClass::macAddress(uint8_t* mac)
{
    mmwlan_get_mac_addr(mac);
    return mac;
}

/**
 * Get the station interface MAC address.
 * @return String mac
 */
String HalowSTAClass::macAddress(void)
{
    uint8_t mac[6];
    char macStr[18] = { 0 };
    mmwlan_get_mac_addr(mac);
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
}

/**
 * Get the interface subnet mask address.
 * @return IPAddress subnetMask
 */
IPAddress HalowSTAClass::subnetMask()
{
  if (status() != WL_CONNECTED)
    return IPAddress();
  return IPAddress(lwip_mmnetif.netmask.u_addr.ip4.addr);
}

/**
 * Get the gateway ip address.
 * @return IPAddress gatewayIP
 */
IPAddress HalowSTAClass::gatewayIP()
{
  if (status() != WL_CONNECTED)
    return IPAddress();
  return IPAddress(lwip_mmnetif.gw.u_addr.ip4.addr);
}



/**
 * Get the broadcast ip address.
 * @return IPAddress broadcastIP
 */
IPAddress HalowSTAClass::broadcastIP()
{
	if (status() != WL_CONNECTED)
	  return IPAddress();

    return HalowGenericClass::calculateBroadcast(gatewayIP(), subnetMask());
}

/**
 * Get the network id.
 * @return IPAddress networkID
 */
IPAddress HalowSTAClass::networkID()
{
	if (status() != WL_CONNECTED)
	  return IPAddress();

    return HalowGenericClass::calculateNetworkID(gatewayIP(), subnetMask());
}

/**
 * Get the subnet CIDR.
 * @return uint8_t subnetCIDR
 */
uint8_t HalowSTAClass::subnetCIDR()
{
	if (status() != WL_CONNECTED)
	  return IPAddress();

    return HalowGenericClass::calculateSubnetCIDR(subnetMask());
}


/**
 * Return the current SSID associated with the network
 * @return SSID
 */
String HalowSTAClass::SSID() const
{
	return String((char *)sta_args.ssid);
}

/**
 * Return the current pre shared key associated with the network
 * @return  psk string
 */
String HalowSTAClass::psk() const
{
	return String(sta_args.passphrase);
}

/**
 * Return the current bssid / mac associated with the network if configured
 * @return bssid uint8_t *
 */
uint8_t* HalowSTAClass::BSSID(uint8_t* buff)
{
    static uint8_t bssid[6];
    enum mmwlan_status err=mmwlan_get_bssid(bssid);
    //Serial.printf("err %d, %02X:%02X:%02X:%02X:%02X:%02X", err,bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    if (buff != NULL) {
        if(err != MMWLAN_SUCCESS) {
          memset(buff, 0, 6);
        } else {
          memcpy(buff, bssid, 6);
        }
        return  buff;
    }
    if(err == MMWLAN_SUCCESS) {
        return reinterpret_cast<uint8_t*>(bssid);
    }
    return NULL;
}

/**
 * Return the current bssid / mac associated with the network if configured
 * @return String bssid mac
 */
String HalowSTAClass::BSSIDstr(void)
{
    uint8_t* bssid = BSSID();
    if(!bssid){
        return String();
    }
    char mac[18] = { 0 };
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    return String(mac);
}

/**
 * Return the current network RSSI.
 * @return  RSSI value
 */
int8_t HalowSTAClass::RSSI(void)
{

	if (status() != WL_CONNECTED) {
	  return -127;
	}

    return (int8_t)mmwlan_get_rssi();
}


//HalowSTAClass HaLow;

