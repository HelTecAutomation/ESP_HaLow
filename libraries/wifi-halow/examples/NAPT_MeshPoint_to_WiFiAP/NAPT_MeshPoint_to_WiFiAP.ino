/*
 * NAPT: HaLow mesh point -> WiFi AP.
 *
 * Topology:
 *
 *   HaLow mesh gateway
 *          ^
 *          | mesh point
 *       ESP32
 *          | WiFi AP + DHCP server
 *          v 
 *      WiFi clients
 *
 * The ESP32 joins a HaLow mesh as a point (DHCP or static IP), creates a WiFi
 * AP for local clients, and enables NAPT between the WiFi AP subnet and the
 * mesh uplink.
 */
#include <HaLow.h>
#include "WiFi.h"

#include <Arduino.h>
#include <esp_netif.h>
#include <esp_netif_net_stack.h>
#include <lwip/netif.h>
#include <dhcpserver/dhcpserver.h>
#include "mmwlan.h"
#include "mmipal.h"

/* ------------------------ Configuration options ------------------------ */

#define HALOW_REGION       "US"
#define MESH_ID            "HaLow-Mesh"
#define MESH_PASSWORD      "heltec.org"
#define MESH_CHANNEL       28
#define MESH_BANDWIDTH     8

/*
 * Mesh point IP mode:
 *   1 = DHCP client
 *   0 = static IP
 */
#ifndef HALOW_USE_DHCP
#define HALOW_USE_DHCP 1
#endif

#if !HALOW_USE_DHCP
IPAddress local_IP(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);
#endif

#define AP_SSID         "MeshPoint-AP"
#define AP_PASSWORD     "12345678"
#define AP_CHANNEL      6
#define UPSTREAM_DNS    "8.8.8.8"

/* ------------------------ End of configuration options ------------------------ */

static bool softap_with_napt()
{
  if (!WiFi.mode(WIFI_AP))
  {
    Serial.println("WiFi.mode(WIFI_AP) failed");
    return false;
  }
  delay(10);

  esp_netif_t *ap_netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
  if (ap_netif != NULL)
  {
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
  if (!WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL))
  {
    Serial.println("softAP failed");
    return false;
  }

  Serial.print("WiFi AP up: SSID=");
  Serial.print(AP_SSID);
  Serial.print(" IP=");
  Serial.println(WiFi.softAPIP());

  delay(10);
  esp_err_t err = esp_netif_napt_enable(ap_netif);
  if (err != ESP_OK)
  {
    Serial.printf("esp_netif_napt_enable failed: %s (0x%x)\r\n",
                  esp_err_to_name(err), err);
    return false;
  }
  Serial.println("NAPT enabled on WiFi AP");

  delay(10);
  netif_set_default(HaLow.netif());
  Serial.println("Default route set to mesh netif");

  return true;
}

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.printf("\r\nMesh point: %s\r\n", MESH_ID);
  Serial.printf("Mesh point IP mode: %s\r\n", HALOW_USE_DHCP ? "DHCP" : "static");

#ifdef HT_RC3268
  pinMode(HALOW_LDO_CTRL, OUTPUT);
  digitalWrite(HALOW_LDO_CTRL, HALOW_LDO_ENABLE);
#endif

#if !HALOW_USE_DHCP
  if (!HaLow.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("Mesh point static IP configuration failed");
  }
#endif

  if (!HaLow.meshBegin(MESH_ID, MESH_PASSWORD, MESH_CHANNEL,
                       MESH_BANDWIDTH, HALOW_REGION))
  {
    Serial.println("Failed to start mesh point");
    return;
  }

  while (HaLow.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Mesh point connected");
  Serial.print("Mesh IP: ");
  Serial.println(HaLow.localIP());
  Serial.print("Mesh MAC: ");
  Serial.println(HaLow.macAddress());
  Serial.print("Mesh Gateway: ");
  Serial.println(HaLow.gatewayIP());

  softap_with_napt();
}

void loop()
{
  static uint32_t last_report = 0;
  uint32_t now = millis();

  if (now - last_report >= 5000)
  {
    last_report = now;
    Serial.printf("[NAPT] WiFi AP=%s Mesh=%s\r\n",
                  WiFi.softAPIP().toString().c_str(),
                  HaLow.localIP().toString().c_str());
  }

  delay(100);
}
