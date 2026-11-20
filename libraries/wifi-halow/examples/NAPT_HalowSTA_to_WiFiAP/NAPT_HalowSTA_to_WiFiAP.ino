#include <HaLow.h>
#include "WiFi.h"

#include <Arduino.h>
#include <esp_netif.h>
#include <esp_netif_net_stack.h>
#include <lwip/netif.h>
#include <dhcpserver/dhcpserver.h>
#include "mmwlan.h"
#include "mmipal.h"

#define HALOW_REGION     "US"
#define HALOW_AP_SSID    "HaLow-AP-137"
#define HALOW_PASSWORD   "12345678"

/*
 * HaLow STA IP mode:
 *   1 = DHCP client
 *   0 = static IP (configure the addresses below)
 */
#ifndef HALOW_USE_DHCP
#define HALOW_USE_DHCP 1
#endif

#if !HALOW_USE_DHCP
IPAddress local_IP(192, 168, 1, 218);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);    // optional
IPAddress secondaryDNS(8, 8, 4, 4);  // optional
#endif

#define TAG "HALOW"

#define AP_SSID         "NaptTest"
#define AP_PASSWORD     "12345678"
#define UPSTREAM_DNS    "8.8.8.8"   // your HaLow side's router or 8.8.8.8
#define AP_CHANNEL      6


static bool softap_with_napt() {
    if (!WiFi.mode(WIFI_AP)) {
        Serial.printf("[%s] WiFi.mode(WIFI_AP) failed\n", TAG);
        return false;
    }
    delay(10);
    esp_netif_t* ap_netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    if (ap_netif != nullptr) {
        Serial.println("set dns");
        dhcps_offer_t offer_dns = OFFER_DNS;
        esp_netif_dhcps_option(ap_netif, ESP_NETIF_OP_SET,
                               ESP_NETIF_DOMAIN_NAME_SERVER,
                               &offer_dns, sizeof(offer_dns));

        esp_netif_dns_info_t dns_info = {};
        dns_info.ip.u_addr.ip4.addr = ipaddr_addr(UPSTREAM_DNS);
        dns_info.ip.type = ESP_IPADDR_TYPE_V4;
        esp_netif_set_dns_info(ap_netif, ESP_NETIF_DNS_MAIN, &dns_info);
    }

    delay(10);
    if (!WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL)) {
        Serial.printf("[%s] softAP failed\n", TAG);
        return false; 
    }
    Serial.printf("[%s] softAP up: SSID=%s IP=%s\n",
                  TAG, AP_SSID, WiFi.softAPIP().toString().c_str());

    /* Enable NAPT on the AP netif */
    delay(10);
    esp_err_t err = esp_netif_napt_enable(ap_netif);
    if (err != ESP_OK) {
        Serial.printf("[%s] esp_netif_napt_enable failed: %s (0x%x)\n",
                      TAG, esp_err_to_name(err), err);
        return false;
    }
    Serial.printf("[%s] NAPT enabled on AP netif\n", TAG);

    /* HaLow as default route */
    delay(10);
    netif_set_default(HaLow.netif());
    Serial.printf("[%s] default route set to HaLow netif\n", TAG);

    return true;
}


static void HaLowEvent(HaLowEvent_t event)
{
  Serial.printf("[HaLow-event] event: %d - %s\r\n", event, HaLow.eventName(event));
}

static void printHalowLink()
{
  uint8_t bssid[6];
  struct mmipal_sta_rssi info;
  bool have_info = false;

  if (HaLow.BSSID(bssid) != NULL &&
      mmipal_get_sta_rssi(bssid, &info) == MMIPAL_SUCCESS)
  {
    have_info = true;
  }

  int8_t rssi = have_info ? info.rssi : HaLow.RSSI();

  Serial.printf("[HaLow-link] rssi=%d dBm", rssi);
  if (have_info)
  {
    if (info.noise_dbm > -128 && info.noise_dbm < 0)
    {
      Serial.printf(" noise=%d dBm", info.noise_dbm);
    }
    else
    {
      Serial.print(" noise=n/a");
    }
    if (info.bw_mhz > 0)
    {
      Serial.printf(" bw=%u MHz", info.bw_mhz);
    }
    else
    {
      Serial.print(" bw=n/a");
    }
    if (info.freq_100khz > 0)
    {
      Serial.printf(" freq=%u.%u MHz",
                    (unsigned)(info.freq_100khz / 10),
                    (unsigned)(info.freq_100khz % 10));
    }
    else
    {
      Serial.print(" freq=n/a");
    }
    Serial.printf(" last_rx=%lu ms", (unsigned long)info.last_rx_ms);
  }
  else
  {
    Serial.print(" noise=n/a bw=n/a freq=n/a");
  }
  Serial.println();
}

void setup()
{
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(HALOW_AP_SSID);
  Serial.printf("[%s] HaLow STA IP mode: %s\n", TAG,
                HALOW_USE_DHCP ? "DHCP" : "STATIC");

#ifdef HT_RC3268
  //enable WiFiHalow LDO
  pinMode(HALOW_LDO_CTRL,OUTPUT);
  digitalWrite(HALOW_LDO_CTRL,HALOW_LDO_ENABLE);
#endif

#if !HALOW_USE_DHCP
  if (!HaLow.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("HaLow static IP configuration failed");
  }
#endif

  HaLow.onEvent(HaLowEvent);
  HaLow.init(HALOW_REGION);
  HaLow.begin(HALOW_AP_SSID, HALOW_PASSWORD);

  while (HaLow.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("HaLow connected!");
  Serial.print("IP address: ");
  Serial.println(HaLow.localIP());
  Serial.print("Mac Address: ");
  Serial.println(HaLow.macAddress());
  Serial.print("Subnet Mask: ");
  Serial.println(HaLow.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(HaLow.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(HaLow.dnsIP());

  delay(20);
  softap_with_napt();  
}


void loop()
{
  static uint32_t last_report = 0;
  uint32_t now = millis();

  if (now - last_report >= 5000)
  {
    last_report = now;
    Serial.printf("[%u][NAPT] HaLow=%s WiFiAP=%s\r\n",millis()/1000,
                  HaLow.localIP().toString().c_str(),
                  WiFi.softAPIP().toString().c_str());
    printHalowLink();
  }

  delay(100);
}
