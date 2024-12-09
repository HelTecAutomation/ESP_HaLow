/*
 HaLowScan.cpp - HaLowScan library for esp32

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

#include "Halow.h"
#include "HalowGeneric.h"
#include "HalowScan.h"

extern "C" {
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <esp_err.h>
#include <esp_event.h>
#include <esp32-hal.h>
#include <lwip/ip_addr.h>
#include "lwip/err.h"
}

#define HALOW_SCAN_RUNNING   (-1)
#define HALOW_SCAN_FAILED    (-2)

#define MAX_SCAN_RECORD_NUM 10

/** ANSI escape sequence for bold text (disabled so no-op). */
#define ANSI_BOLD  ""
/** ANSI escape sequence to reset font (disabled so no-op). */
#define ANSI_RESET ""


/** Length of string representation of a MAC address (i.e., "XX:XX:XX:XX:XX:XX")
 * including null terminator. */
#define MAC_ADDR_STR_LEN    (18)

/** Number of results found. */
static int num_scan_results=0;

/** Enumeration of Authentication Key Management (AKM) Suite OUIs as BE32 integers. */
enum akm_suite_oui
{
    /** Open (no security) */
    AKM_SUITE_NONE = 0,
    /** Pre-shared key (WFA OUI) */
    AKM_SUITE_PSK = 0x506f9a02,
    /** Simultaneous Authentication of Equals (SAE) */
    AKM_SUITE_SAE = 0x000fac08,
    /** OWE */
    AKM_SUITE_OWE = 0x000fac12,
    /** Another suite not in this enum */
    AKM_SUITE_OTHER = 1,
};

/**
 * Get the name of the given AKM Suite as a string.
 *
 * @param akm_suite_oui     The OUI of the AKM suite as a big endian integer.
 *
 * @returns the string representation.
 */
const char *akm_suite_to_string(uint32_t akm_suite_oui)
{
    switch (akm_suite_oui)
    {
    case AKM_SUITE_NONE:
        return "None";

    case AKM_SUITE_PSK:
        return "PSK";

    case AKM_SUITE_SAE:
        return "SAE";

    case AKM_SUITE_OWE:
        return "OWE";

    default:
        return "Other";
    }
}

/** Result of the scan request. */
typedef struct halow_scan_result
{
    /** RSSI of the received frame. */
    int16_t rssi;
    /** Pointer to the BSSID field within the Probe Response frame. */
    uint8_t bssid[6];
    /** Pointer to the SSID within the SSID IE of the Probe Response frame. */
    char ssid[33];
    /** Value of the Beacon Interval field. */
    uint16_t beacon_interval;
    /** Value of the Capability Information  field. */
    uint16_t capability_info;
    /** Length of the SSID (@c ssid). */
    uint8_t ssid_len;
    /** Operating bandwidth, in MHz, of the access point. */
    uint8_t op_bw_mhz;
    mmwlan_security_type_t security_type;
}halow_scan_result_t;


/** Maximum number of pairwise cipher suites our parser will process. */
#define RSN_INFORMATION_MAX_PAIRWISE_CIPHER_SUITES  (2)

/** Maximum number of AKM suites our parser will process. */
#define RSN_INFORMATION_MAX_AKM_SUITES  (2)

halow_scan_result_t halow_ap_record[MAX_SCAN_RECORD_NUM];


/**
 * Data structure to represent information extracted from an RSN information element.
 *
 * All integers in host order.
 */
struct rsn_information
{
    /** The group cipher suite OUI. */
    uint32_t group_cipher_suite;
    /** Pairwise cipher suite OUIs. Count given by @c num_pairwise_cipher_suites. */
    uint32_t pairwise_cipher_suites[RSN_INFORMATION_MAX_PAIRWISE_CIPHER_SUITES];
    /** AKM suite OUIs. Count given by @c num_akm_suites. */
    uint32_t akm_suites[RSN_INFORMATION_MAX_AKM_SUITES];
    /** Number of pairwise cipher suites in @c pairwise_cipher_suites. */
    uint16_t num_pairwise_cipher_suites;
    /** Number of AKM suites in @c akm_suites. */
    uint16_t num_akm_suites;
    /** Version number of the RSN IE. */
    uint16_t version;
    /** RSN Capabilities field of the RSN IE (in host order). */
    uint16_t rsn_capabilities;
};


/** Tag number of the RSN information element, in which we can find security details of the AP. */
#define RSN_INFORMATION_IE_TYPE (48)

/**
 * Search through the given list of information elements to find the RSN IE then parse it
 * to extract relevant information into an instance of @ref rsn_information.
 *
 * @param[in] ies       Buffer containing the information elements.
 * @param[in] ies_len   Length of @p ies
 * @param[out] output   Pointer to an instance of @ref rsn_information to receive output.
 *
 * @returns -1 on parse error, 0 if the RSN IE was not found, 1 if the RSN IE was found.
 */
static int parse_rsn_information(const uint8_t *ies, unsigned ies_len,
                                 struct rsn_information *output)
{
    size_t offset = 0;
    memset(output, 0, sizeof(*output));

    while (offset < ies_len)
    {
        uint8_t type = ies[offset++];
        uint8_t length = ies[offset++];

        if (type == RSN_INFORMATION_IE_TYPE)
        {
            uint16_t num_pairwise_cipher_suites;
            uint16_t num_akm_suites;
            uint16_t ii;


            if (offset + length > ies_len)
            {
                printf("*WRN* RSN IE extends past end of IEs\n");
                return -1;
            }

            if (length < 8)
            {
                printf("*WRN* RSN IE too short\n");
                return -1;
            }

            /* Skip version field */
            output->version = ies[offset] | ies[offset+1] << 8;
            offset += 2;
            length -= 2;

            output->group_cipher_suite =
                ies[offset] << 24 | ies[offset+1] << 16 | ies[offset+2] << 8 | ies[offset+3];
            offset += 4;
            length -= 4;

            num_pairwise_cipher_suites = ies[offset] | ies[offset+1] << 8;
            offset += 2;
            length -= 2;

            output->num_pairwise_cipher_suites = num_pairwise_cipher_suites;
            if (num_pairwise_cipher_suites > RSN_INFORMATION_MAX_PAIRWISE_CIPHER_SUITES)
            {
                output->num_pairwise_cipher_suites = RSN_INFORMATION_MAX_PAIRWISE_CIPHER_SUITES;
            }

            if (length < 4 * num_pairwise_cipher_suites + 2)
            {
                printf("*WRN* RSN IE too short\n");
                return -1;
            }

            for (ii = 0; ii < num_pairwise_cipher_suites; ii++)
            {
                if (ii < output->num_pairwise_cipher_suites)
                {
                    output->pairwise_cipher_suites[ii] =
                        ies[offset] << 24 | ies[offset+1] << 16 |
                        ies[offset+2] << 8 | ies[offset+3];
                }
                offset += 4;
                length -= 4;
            }

            num_akm_suites = ies[offset] | ies[offset+1] << 8;
            offset += 2;
            length -= 2;

            output->num_akm_suites = num_akm_suites;
            if (num_akm_suites > RSN_INFORMATION_MAX_AKM_SUITES)
            {
                output->num_akm_suites = RSN_INFORMATION_MAX_AKM_SUITES;
            }

            if (length < 4 * num_akm_suites + 2)
            {
                printf("*WRN* RSN IE too short\n");
                return -1;
            }

            for (ii = 0; ii < num_akm_suites; ii++)
            {
                if (ii < output->num_akm_suites)
                {
                    output->akm_suites[ii] =
                        ies[offset] << 24 | ies[offset+1] << 16 |
                        ies[offset+2] << 8 | ies[offset+3];
                }
                offset += 4;
                length -= 4;
            }

            output->rsn_capabilities = ies[offset] | ies[offset+1] << 8;
            return 1;
        }

        offset += length;
    }

    /* No RSE IE found; implies open security. */
    return 0;
}


/**
 * Scan rx callback.
 *
 * @param result        Pointer to the scan result.
 * @param arg           Opaque argument.
 */

 
static void scan_rx_callback(const struct mmwlan_scan_result *result, void *arg)
{
    (void)(arg);
    char bssid_str[MAC_ADDR_STR_LEN];
    char ssid_str[MMWLAN_SSID_MAXLEN];
    int ret;
    struct rsn_information rsn_info;
    if(num_scan_results==(MAX_SCAN_RECORD_NUM - 1))
    	return;
    for(int k=0;k<num_scan_results;k++)
    {
       if( memcmp(&(halow_ap_record[k].bssid),result->bssid,6)==0)
       {
          return;
       }
    }

    halow_ap_record[num_scan_results].ssid_len=result->ssid_len;
    halow_ap_record[num_scan_results].op_bw_mhz=result->op_bw_mhz;
    halow_ap_record[num_scan_results].beacon_interval=result->beacon_interval;
    halow_ap_record[num_scan_results].rssi=result->rssi;
    halow_ap_record[num_scan_results].capability_info=result->capability_info;

    memcpy(&(halow_ap_record[num_scan_results].bssid),result->bssid,6);
    (void)mmosal_safer_strcpy((char *)&(halow_ap_record[num_scan_results].ssid), (char *)result->ssid, sizeof(halow_ap_record[num_scan_results].ssid));
    if(result->ssid_len<sizeof(halow_ap_record[num_scan_results].ssid))
    {
      halow_ap_record[num_scan_results].ssid[result->ssid_len]=0;
    }
    ret = parse_rsn_information(result->ies, result->ies_len, &rsn_info);
    if (ret < 0)
    {
        halow_ap_record[num_scan_results].security_type=MMWLAN_OTHER;
    }
    else if (rsn_info.num_akm_suites == 0)
    {
        halow_ap_record[num_scan_results].security_type=MMWLAN_OPEN;
    }
    else if (ret > 0)
    {
        unsigned ii;
        if(rsn_info.akm_suites[0]==AKM_SUITE_SAE)
        {
          halow_ap_record[num_scan_results].security_type=MMWLAN_SAE;
        }
        else if((rsn_info.akm_suites[0]==AKM_SUITE_OWE))
        {
          halow_ap_record[num_scan_results].security_type=MMWLAN_OWE;
        }
        else if((rsn_info.akm_suites[0]==AKM_SUITE_NONE))
        {
          halow_ap_record[num_scan_results].security_type=MMWLAN_OPEN;
        }
        else
        {
          halow_ap_record[num_scan_results].security_type=MMWLAN_OTHER;
        }
    }


    num_scan_results++;


}


/**
 * Scan complete callback.
 *
 * @param state         Scan complete status.
 * @param arg           Opaque argument.
 */
static void scan_complete_callback(enum mmwlan_scan_state state, void *arg)
{
    (void)(state);
    (void)(arg);
    printf("Scanning completed.\n");
    esp_event_post(HALOW_EVENTS, HALOW_EVENT_SCAN_DONE, NULL, 0, portMAX_DELAY);
}



bool HalowScanClass::_scanAsync = false;
uint32_t HalowScanClass::_scanStarted = 0;
uint32_t HalowScanClass::_scanTimeout = 10000;
uint16_t HalowScanClass::_scanCount = 0;
void* HalowScanClass::_scanResult = 0;

/**
 * Start scan Halow networks available
 * @param async         run in async mode
 * @param show_hidden   show hidden networks
 * @return Number of discovered networks
 */
int16_t HalowScanClass::scanNetworks(bool async, bool show_hidden, bool passive, uint32_t max_ms_per_chan, uint8_t channel, const char * ssid, const uint8_t * bssid)
{
	if(HalowGenericClass::getStatusBits() & HALOW_SCANNING_BIT) {
		return HALOW_SCAN_RUNNING;
	}
	enum mmwlan_status _status;
	num_scan_results = 0;
	HalowScanClass::_scanStarted=0;
	struct mmwlan_scan_req scan_req = MMWLAN_SCAN_REQ_INIT;
	scan_req.scan_rx_cb = scan_rx_callback;
	scan_req.scan_complete_cb = scan_complete_callback;
	_status = mmwlan_scan_request(&scan_req);
	MMOSAL_ASSERT(_status == MMWLAN_SUCCESS);
	if(_status == MMWLAN_SUCCESS)
	{
		_scanStarted = millis();
		if (!_scanStarted) { //Prevent 0 from millis overflow
			++_scanStarted;
		}

		HalowGenericClass::clearStatusBits(HALOW_SCAN_DONE_BIT);
		HalowGenericClass::setStatusBits(HALOW_SCANNING_BIT);

		if(HalowScanClass::_scanAsync) {
			return HALOW_SCAN_RUNNING;
		}
		if(HalowGenericClass::waitStatusBits(HALOW_SCAN_DONE_BIT, 10000)){
			return (int16_t) HalowScanClass::_scanCount;
		}
	}
	return HALOW_SCAN_FAILED;
}


/**
 * private
 * scan callback
 * @param result  void *arg
 * @param status STATUS
 */
void HalowScanClass::_scanDone()
{
    HalowScanClass::_scanCount=num_scan_results;
    HalowScanClass::_scanStarted=0; //Reset after a scan is completed for normal behavior
    HalowGenericClass::setStatusBits(HALOW_SCAN_DONE_BIT);
    HalowGenericClass::clearStatusBits(HALOW_SCANNING_BIT);
}

/**
 *
 * @param i specify from which network item want to get the information
 * @return bss_info *
 */
void * HalowScanClass::_getScanInfoByIndex(int i)
{
    return 0;
}

/**
 * called to get the scan state in Async mode
 * @return scan result or status
 *          -1 if scan not fin
 *          -2 if scan not triggered
 */
int16_t HalowScanClass::scanComplete()
{
    if(HalowGenericClass::getStatusBits() & HALOW_SCAN_DONE_BIT) {
        return HalowScanClass::_scanCount;
    }

    if(HalowGenericClass::getStatusBits() & HALOW_SCANNING_BIT) {
        return HALOW_SCAN_RUNNING;
    }
    // last one to avoid time affecting Async mode
    if (HalowScanClass::_scanStarted && (millis()-HalowScanClass::_scanStarted) > HalowScanClass::_scanTimeout) { //Check is scan was started and if the delay expired, return HALOW_SCAN_FAILED in this case 
    	HalowGenericClass::clearStatusBits(HALOW_SCANNING_BIT);
	return HALOW_SCAN_FAILED;
    }

    return HALOW_SCAN_FAILED;
}

/**
 * delete last scan result from RAM
 */
void HalowScanClass::scanDelete()
{
    HalowGenericClass::clearStatusBits(HALOW_SCAN_DONE_BIT);
    if(HalowScanClass::_scanResult) {
        num_scan_results=0;
        HalowScanClass::_scanResult = 0;
        HalowScanClass::_scanCount = 0;
    }
}

/**
 * Return the SSID discovered during the network scan.
 * @param i     specify from which network item want to get the information
 * @return       ssid string of the specified item on the networks scanned list
 */
String HalowScanClass::SSID(uint8_t i)
{
    if(i<HalowScanClass::_scanCount)
    {
      return String(halow_ap_record[i].ssid);
    }
    return String();
}


/**
 * Return the encryption type of the networks discovered during the scanNetworks
 * @param i specify from which network item want to get the information
 * @return  encryption type (enum wl_enc_type) of the specified item on the networks scanned list
 */
mmwlan_security_type_t HalowScanClass::encryptionType(uint8_t i)
{
    if(i<HalowScanClass::_scanCount)
    {
      return halow_ap_record[i].security_type;
    }
    return MMWLAN_OTHER;
}


String HalowScanClass::encryptionTypeStr(uint8_t i)
{
	switch(encryptionType(i))
	{
	case MMWLAN_OPEN:
		return String("OPEN");
		break;
	case MMWLAN_OWE:
		return String("OWE");
		break;
	case MMWLAN_SAE:
		return String("SAE");
		break;
	case MMWLAN_OTHER:
	default:
		return String("UNKNOWN");
	}
}
/**
 * Return the RSSI of the networks discovered during the scanNetworks
 * @param i specify from which network item want to get the information
 * @return  signed value of RSSI of the specified item on the networks scanned list
 */
int32_t HalowScanClass::RSSI(uint8_t i)
{
    if(i<HalowScanClass::_scanCount)
    {
      return halow_ap_record[i].rssi;
    }
    return 0;
}


/**
 * return MAC / BSSID of scanned Halow
 * @param i specify from which network item want to get the information
 * @param buff optional buffer for the result uint8_t array with length 6
 * @return uint8_t * MAC / BSSID of scanned Halow
 */
uint8_t * HalowScanClass::BSSID(uint8_t i, uint8_t* buff)
{
    if (buff != NULL) {
        if(i<HalowScanClass::_scanCount) {
          memcpy(buff, halow_ap_record[i].bssid, 6);
        } else {
          memset(buff, 0, 6);
        }
        return  buff;
    }
    if(i<HalowScanClass::_scanCount) {
        return reinterpret_cast<uint8_t*>(halow_ap_record[i].bssid);
    }
    return NULL;

}

/**
 * return MAC / BSSID of scanned Halow
 * @param i specify from which network item want to get the information
 * @return String MAC / BSSID of scanned Halow
 */
String HalowScanClass::BSSIDstr(uint8_t i)
{
    uint8_t* bssid = BSSID(i);

    if(!bssid)
    {
      return String();
    }
    char mac[18] = { 0 };
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    return String(mac);
}

int32_t HalowScanClass::channel(uint8_t i)
{
    return 0;
}

