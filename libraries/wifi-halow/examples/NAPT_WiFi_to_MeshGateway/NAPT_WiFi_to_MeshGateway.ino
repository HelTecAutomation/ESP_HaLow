/*
 * NAPT: WiFi STA uplink -> HaLow mesh gateway.
 *
 * Topology:
 *
 *   WiFi AP / Internet
 *          ^
 *          | WiFi STA
 *       ESP32
 *          | mesh gateway + DHCP server
 *          v
 *      HaLow mesh points
 *
 * The ESP32 connects to a WiFi AP as STA, creates a HaLow mesh gateway on
 * 192.168.1.1/24, starts DHCP for mesh points, and enables NAPT between the
 * mesh subnet and the WiFi uplink.
 */
#include <HaLow.h>
#include "WiFi.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "esp_netif.h"
#include "esp_netif_net_stack.h"

/* ------------------------ Configuration options ------------------------ */

#define HALOW_REGION       "US"
#define MESH_ID            "HaLow-Mesh"
#define MESH_PASSWORD      "heltec.org"
#define MESH_CHANNEL       28
#define MESH_BANDWIDTH     8

#define WIFI_STA_SSID      "HT-HD02-7F82"
#define WIFI_STA_PASSWORD  "heltec.org"

IPAddress gateway_ip(192, 168, 1, 1);
IPAddress netmask(255, 255, 255, 0);
IPAddress dns_primary(8, 8, 8, 8);
IPAddress dns_secondary(8, 8, 4, 4);

IPAddress dhcp_lease_start(192, 168, 1, 100);
IPAddress dhcp_lease_end(192, 168, 1, 150);

/* ------------------------ End of configuration options ------------------------ */

static void enable_mesh_napt(void *arg)
{
  (void)arg;

  esp_netif_t *sta_netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
  if (sta_netif != NULL)
  {
    struct netif *sta = (struct netif *)esp_netif_get_netif_impl(sta_netif);
    if (sta != NULL)
    {
      netif_set_default(sta);
    }
  }

  ip_napt_enable((uint32_t)gateway_ip, 1);
  Serial.println("NAPT enabled: WiFi STA <-> mesh gateway");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
  (void)event;
  Serial.print("WiFi got IP: ");
  Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));

  if (tcpip_callback(enable_mesh_napt, NULL) != ERR_OK)
  {
    Serial.println("Failed to schedule NAPT enable");
  }
}

void setup()
{
  Serial.begin(115200);
  delay(100);

#ifdef HT_RC3268
  pinMode(HALOW_LDO_CTRL, OUTPUT);
  digitalWrite(HALOW_LDO_CTRL, HALOW_LDO_ENABLE);
#endif

  Serial.printf("\r\nMesh gateway: %s\r\n", MESH_ID);

  if (!HaLow.config(gateway_ip, gateway_ip, netmask, dns_primary, dns_secondary))
  {
    Serial.println("Mesh gateway static IP configuration failed");
  }

  if (!HaLow.meshBegin(MESH_ID, MESH_PASSWORD, MESH_CHANNEL,
                       MESH_BANDWIDTH, HALOW_REGION))
  {
    Serial.println("Failed to start mesh gateway");
    return;
  }

  while (HaLow.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Mesh gateway connected");
  Serial.print("Mesh IP: ");
  Serial.println(HaLow.localIP());
  Serial.print("Mesh MAC: ");
  Serial.println(HaLow.macAddress());

  if (HaLow.enableDhcpServer(dhcp_lease_start, dhcp_lease_end, dns_primary))
  {
    Serial.printf("Mesh DHCP server: %s - %s\r\n",
                  dhcp_lease_start.toString().c_str(),
                  dhcp_lease_end.toString().c_str());
  }
  else
  {
    Serial.println("Mesh DHCP server start failed");
  }

  WiFi.mode(WIFI_STA);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASSWORD);

  Serial.print("Connecting WiFi: ");
  Serial.println(WIFI_STA_SSID);
}

void loop()
{
  static uint32_t last_report = 0;
  uint32_t now = millis();

  if (now - last_report >= 5000)
  {
    last_report = now;
    Serial.printf("[NAPT] WiFi=%s Mesh=%s\r\n",
                  WiFi.localIP().toString().c_str(),
                  HaLow.localIP().toString().c_str());
  }

  delay(100);
}
