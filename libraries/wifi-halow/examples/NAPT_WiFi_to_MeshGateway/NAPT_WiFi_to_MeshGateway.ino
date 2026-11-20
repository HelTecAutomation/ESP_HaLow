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

#ifndef MESH_PEER_WHITELIST_ENABLE
#define MESH_PEER_WHITELIST_ENABLE 0
#endif
#ifndef MESH_PEER_WHITELIST_0
#define MESH_PEER_WHITELIST_0 ""
#endif
#ifndef MESH_PEER_WHITELIST_1
#define MESH_PEER_WHITELIST_1 ""
#endif
#ifndef MESH_PEER_WHITELIST_2
#define MESH_PEER_WHITELIST_2 ""
#endif
#ifndef MESH_PEER_WHITELIST_3
#define MESH_PEER_WHITELIST_3 ""
#endif

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

static bool configure_mesh_whitelist(void)
{
#if MESH_PEER_WHITELIST_ENABLE
    static const char *const configured[] = {
        MESH_PEER_WHITELIST_0,
        MESH_PEER_WHITELIST_1,
        MESH_PEER_WHITELIST_2,
        MESH_PEER_WHITELIST_3,
    };
    uint8_t peers[MMWLAN_MESH_PEER_WHITELIST_MAX][MMWLAN_MAC_ADDR_LEN];
    size_t peer_count = 0;

    for (size_t i = 0; i < MMWLAN_MESH_PEER_WHITELIST_MAX; i++)
    {
        if (configured[i][0] == '\0')
        {
            continue;
        }

        unsigned value[MMWLAN_MAC_ADDR_LEN];
        if (sscanf(configured[i], "%x:%x:%x:%x:%x:%x",
                   &value[0], &value[1], &value[2],
                   &value[3], &value[4], &value[5]) != MMWLAN_MAC_ADDR_LEN)
        {
            Serial.printf("[MESH ACL] invalid whitelist MAC: %s\r\n", configured[i]);
            return false;
        }

        bool all_zero = true;
        for (size_t j = 0; j < MMWLAN_MAC_ADDR_LEN; j++)
        {
            if (value[j] > 0xFF)
            {
                Serial.printf("[MESH ACL] invalid whitelist MAC: %s\r\n", configured[i]);
                return false;
            }
            peers[peer_count][j] = (uint8_t)value[j];
            if (peers[peer_count][j] != 0)
            {
                all_zero = false;
            }
        }
        if (all_zero || (peers[peer_count][0] & 0x01))
        {
            Serial.printf("[MESH ACL] invalid whitelist MAC: %s\r\n", configured[i]);
            return false;
        }
        peer_count++;
    }

    if (peer_count == 0)
    {
        Serial.println("[MESH ACL] whitelist enabled but no MAC configured");
        return false;
    }

    if (!HaLow.meshSetPeerWhitelist(
            (const uint8_t (*)[MMWLAN_MAC_ADDR_LEN])peers, peer_count))
    {
        Serial.println("[MESH ACL] failed to set whitelist");
        return false;
    }

    Serial.printf("[MESH ACL] whitelist=%u peer(s)\r\n", (unsigned)peer_count);
    for (size_t i = 0; i < peer_count; i++)
    {
        Serial.printf("  %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                      peers[i][0], peers[i][1], peers[i][2],
                      peers[i][3], peers[i][4], peers[i][5]);
    }
#endif
    return true;
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

  if (!configure_mesh_whitelist())
  {
    Serial.println("Mesh whitelist configuration failed");
    return;
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

static void printMeshStatus()
{
  struct mmwlan_mesh_link_stats links[MMWLAN_MESH_LINK_STATS_MAX];
  size_t count = HaLow.meshGetLinkStats(links, MMWLAN_MESH_LINK_STATS_MAX);

  Serial.printf("[MESH] IP=%s MAC=%s RSSI=%d peers=%u\r\n",
                HaLow.localIP().toString().c_str(),
                HaLow.macAddress().c_str(),
                HaLow.RSSI(),
                (unsigned)count);

  for (size_t i = 0; i < count; i++)
  {
    const struct mmwlan_mesh_link_stats *link = &links[i];
    bool zero = true;
    for (size_t j = 0; j < 6; j++)
    {
      if (link->peer[j] != 0)
      {
        zero = false;
        break;
      }
    }
    if (zero)
    {
      continue;
    }

    Serial.printf("  peer=%02X:%02X:%02X:%02X:%02X:%02X rssi_avg=%d min=%d max=%d rx=%lu tx_ok=%lu tx_fail=%lu\r\n",
                  link->peer[0], link->peer[1], link->peer[2],
                  link->peer[3], link->peer[4], link->peer[5],
                  link->rssi_avg, link->rssi_min, link->rssi_max,
                  (unsigned long)link->rx_frames,
                  (unsigned long)link->tx_ok,
                  (unsigned long)link->tx_fail);
  }
}

void loop()
{
  static uint32_t last_report = 0;
  uint32_t now = millis();

  if (now - last_report >= 30000)
  {
    last_report = now;
    printMeshStatus();
    Serial.printf("[NAPT] WiFi=%s Mesh=%s\r\n",
                  WiFi.localIP().toString().c_str(),
                  HaLow.localIP().toString().c_str());
  }

  delay(100);
}
