/*
 * NAPT: Ethernet uplink (W5500) -> HaLow AP.
 *
 * Topology:
 *
 *   Ethernet router / Internet
 *          ^
 *          | W5500 SPI Ethernet
 *       ESP32-S3
 *          | HaLow AP + DHCP server
 *          v
 *      HaLow STAs
 *
 * W5500 pin assignment:
 *   RST  = 10
 *   INT  = 11
 *   CS   = 12
 *   CLK  = 13
 *   MISO = 14
 *   MOSI = 15
 *
 * The W5500 uses HSPI/SPI3, while HaLow uses SPI2, avoiding bus conflicts.
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
#define HALOW_AP_SSID      "HaLow-AP-137"
#define HALOW_PASSWORD     "12345678"
#define HALOW_CHANNEL      28
#define HALOW_BANDWIDTH    8
#define HALOW_MAX_STAS     8

IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

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

  ip_napt_enable((uint32_t)local_IP, 1);
  Serial.println("NAPT enabled: Ethernet -> HaLow AP");
}

static void disable_eth_napt(void *arg)
{
  (void)arg;
  ip_napt_enable((uint32_t)local_IP, 0);
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

  /* HaLow AP setup. */
  if (!HaLow.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("HaLow AP static IP configuration failed");
  }

  HaLow.init(HALOW_REGION);
  if (!HaLow.AP(HALOW_AP_SSID, HALOW_PASSWORD, HALOW_CHANNEL,
                HALOW_BANDWIDTH, HALOW_MAX_STAS))
  {
    Serial.println("Failed to start HaLow AP");
    return;
  }

  while (HaLow.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("HaLow AP started");
  Serial.print("HaLow IP: ");
  Serial.println(HaLow.localIP());

#if ENABLE_DHCP_SERVER
  if (HaLow.enableDhcpServer(dhcp_lease_start, dhcp_lease_end, primaryDNS))
  {
    Serial.printf("HaLow DHCP server: %s - %s\r\n",
                  dhcp_lease_start.toString().c_str(),
                  dhcp_lease_end.toString().c_str());
  }
  else
  {
    Serial.println("HaLow DHCP server start failed");
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
    Serial.printf("[%u][NAPT] ETH=%s HaLow AP=%s\r\n",millis()/1000,
                  ETH.localIP().toString().c_str(),
                  HaLow.localIP().toString().c_str());
  }

  delay(100);
}
