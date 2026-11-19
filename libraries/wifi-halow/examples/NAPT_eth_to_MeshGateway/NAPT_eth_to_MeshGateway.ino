/*
 * NAPT: Ethernet uplink (W5500) -> HaLow mesh gateway.
 *
 * Topology:
 *
 *   Ethernet router / Internet
 *          ^
 *          | W5500 SPI Ethernet
 *       ESP32-S3
 *          | mesh gateway + DHCP server
 *          v
 *      HaLow mesh points
 *
 * W5500 pin assignment:
 *   RST  = 10
 *   INT  = 11
 *   CS   = 12
 *   CLK  = 13
 *   MISO = 14
 *   MOSI = 15
 *
 * W5500 uses HSPI/SPI3; HaLow uses SPI2, so the two buses are independent.
 */
#include <HaLow.h>
#include <ETH.h>
#include <SPI.h>
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

IPAddress gateway_ip(192, 168, 1, 1);
IPAddress netmask(255, 255, 255, 0);
IPAddress dns_primary(8, 8, 8, 8);
IPAddress dns_secondary(8, 8, 4, 4);

IPAddress dhcp_lease_start(192, 168, 1, 100);
IPAddress dhcp_lease_end(192, 168, 1, 150);

/* W5500 SPI Ethernet pin assignment. */
#define ETH_PHY_ADDR   1
#define ETH_PHY_CS     12
#define ETH_PHY_IRQ    11
#define ETH_PHY_RST    10
#define ETH_SPI_SCK    13
#define ETH_SPI_MISO   14
#define ETH_SPI_MOSI   15

/* Use HSPI (SPI3) so W5500 does not share HaLow's SPI2 bus. */
SPIClass ethSPI(HSPI);

#ifndef ENABLE_DHCP_SERVER
#define ENABLE_DHCP_SERVER 1
#endif

/* ------------------------ End of configuration options ------------------------ */

static void enable_eth_napt(void *arg)
{
  (void)arg;

  esp_netif_t *eth_netif = esp_netif_get_handle_from_ifkey("ETH_DEF");
  if (eth_netif != NULL)
  {
    struct netif *eth = (struct netif *)esp_netif_get_netif_impl(eth_netif);
    if (eth != NULL)
    {
      netif_set_default(eth);
    }
  }

  ip_napt_enable((uint32_t)gateway_ip, 1);
  Serial.println("NAPT enabled: Ethernet -> mesh gateway");
}

static void disable_eth_napt(void *arg)
{
  (void)arg;
  ip_napt_enable((uint32_t)gateway_ip, 0);
  Serial.println("NAPT disabled");
}

void onEvent(arduino_event_id_t event, arduino_event_info_t info)
{
  switch (event)
  {
  case ARDUINO_EVENT_ETH_START:
    Serial.println("ETH started");
    break;

  case ARDUINO_EVENT_ETH_CONNECTED:
    Serial.println("ETH link connected");
    break;

  case ARDUINO_EVENT_ETH_GOT_IP:
    Serial.print("ETH got IP: ");
    Serial.println(ETH.localIP());
    Serial.print("ETH gateway: ");
    Serial.println(ETH.gatewayIP());
    if (tcpip_callback(enable_eth_napt, NULL) != ERR_OK)
    {
      Serial.println("Failed to schedule NAPT enable");
    }
    break;

  case ARDUINO_EVENT_ETH_LOST_IP:
  case ARDUINO_EVENT_ETH_DISCONNECTED:
    Serial.println("ETH lost connection");
    if (tcpip_callback(disable_eth_napt, NULL) != ERR_OK)
    {
      Serial.println("Failed to schedule NAPT disable");
    }
    break;

  case ARDUINO_EVENT_ETH_STOP:
    Serial.println("ETH stopped");
    break;

  default:
    break;
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

  /* HaLow mesh gateway setup. */
  if (!HaLow.config(gateway_ip, gateway_ip, netmask,
                    dns_primary, dns_secondary))
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
  Serial.println("Mesh gateway started");
  Serial.print("Mesh IP: ");
  Serial.println(HaLow.localIP());
  Serial.print("Mesh MAC: ");
  Serial.println(HaLow.macAddress());

#if ENABLE_DHCP_SERVER
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
#endif

  /* W5500 Ethernet uplink setup. */
  Network.onEvent(onEvent);

  ethSPI.begin(ETH_SPI_SCK, ETH_SPI_MISO, ETH_SPI_MOSI);

  if (!ETH.begin(ETH_PHY_W5500, ETH_PHY_ADDR, ETH_PHY_CS,
                 ETH_PHY_IRQ, ETH_PHY_RST, ethSPI, 20))
  {
    Serial.println("W5500 Ethernet init failed");
    return;
  }

  Serial.println("W5500 Ethernet started, waiting for DHCP...");
}

void loop()
{
  static uint32_t last_report = 0;
  uint32_t now = millis();

  if (now - last_report >= 30000)
  {
    last_report = now;
    Serial.printf("[%u][NAPT] ETH=%s Mesh=%s\r\n",millis()/1000,
                  ETH.localIP().toString().c_str(),
                  HaLow.localIP().toString().c_str());
  }

  delay(100);
}
