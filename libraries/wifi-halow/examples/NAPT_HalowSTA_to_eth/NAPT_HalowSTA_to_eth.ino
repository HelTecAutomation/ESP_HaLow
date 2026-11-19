/*
 * NAPT: HaLow STA uplink -> Ethernet (W5500) downstream.
 *
 * Topology:
 *
 *   HaLow AP / Internet
 *          ^
 *          | HaLow STA
 *       ESP32-S3
 *          | W5500 Ethernet + DHCP server
 *          v
 *      Ethernet LAN clients
 *
 * W5500 pin assignment:
 *   RST  = 10
 *   INT  = 11
 *   CS   = 12
 *   CLK  = 13
 *   MISO = 14
 *   MOSI = 15
 *
 * The W5500 uses HSPI/SPI3, while HaLow uses SPI2.
 */
#include <HaLow.h>
#include <ETH.h>
#include <SPI.h>

#include <Arduino.h>
#include <esp_netif.h>
#include <esp_netif_net_stack.h>
#include <lwip/netif.h>
#include "lwip/tcpip.h"
#include <lwip/ip_addr.h>
#include <lwip/err.h>
#include <dhcpserver/dhcpserver.h>
#include <dhcpserver/dhcpserver_options.h>
#include "mmwlan.h"
#include "mmipal.h"

/* ------------------------ Configuration options ------------------------ */

#define HALOW_REGION       "US"
#define HALOW_AP_SSID      "HaLow-AP-137"
#define HALOW_PASSWORD     "12345678"

/*
 * HaLow STA IP mode:
 *   1 = DHCP client
 *   0 = static IP
 */
#ifndef HALOW_USE_DHCP
#define HALOW_USE_DHCP 1
#endif

#if !HALOW_USE_DHCP
IPAddress halow_local_IP(192, 168, 1, 218);
IPAddress halow_gateway(192, 168, 1, 1);
IPAddress halow_subnet(255, 255, 255, 0);
IPAddress halow_primaryDNS(8, 8, 8, 8);
IPAddress halow_secondaryDNS(8, 8, 4, 4);
#endif

/* Ethernet downstream configuration. */
IPAddress eth_local_IP(192, 168, 4, 1);
IPAddress eth_gateway(192, 168, 4, 1);
IPAddress eth_subnet(255, 255, 255, 0);
IPAddress eth_primaryDNS(8, 8, 8, 8);
IPAddress eth_secondaryDNS(8, 8, 4, 4);

IPAddress eth_lease_start(192, 168, 4, 100);
IPAddress eth_lease_end(192, 168, 4, 150);

/* W5500 SPI Ethernet pin assignment. */
#define ETH_PHY_ADDR   1
#define ETH_PHY_CS     12
#define ETH_PHY_IRQ    11
#define ETH_PHY_RST    10
#define ETH_SPI_SCK    13
#define ETH_SPI_MISO   14
#define ETH_SPI_MOSI   15

SPIClass ethSPI(HSPI);

#ifndef ENABLE_ETH_DHCP_SERVER
#define ENABLE_ETH_DHCP_SERVER 1
#endif
#ifndef ENABLE_NAPT
#define ENABLE_NAPT 1
#endif

/* ------------------------ End of configuration options ------------------------ */

static dhcps_t *eth_dhcp_server = NULL;

static void eth_dhcp_lease_cb(void *arg, uint8_t ip[4], uint8_t mac[6])
{
  (void)arg;
  Serial.printf("DHCP lease: %u.%u.%u.%u -> %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                ip[0], ip[1], ip[2], ip[3],
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static bool start_eth_dhcp_lwip(void)
{
  if (eth_dhcp_server != NULL)
  {
    return true;
  }

  struct netif *eth_netif = (struct netif *)esp_netif_get_netif_impl(ETH.netif());
  if (eth_netif == NULL)
  {
    Serial.println("Ethernet netif not available");
    return false;
  }

  dhcps_t *server = dhcps_new();
  if (server == NULL)
  {
    Serial.println("dhcps_new failed");
    return false;
  }

  dhcps_lease_t lease;
  lease.enable = true;
  lease.start_ip.addr = (uint32_t)eth_lease_start;
  lease.end_ip.addr = (uint32_t)eth_lease_end;
  dhcps_set_option_info(server, REQUESTED_IP_ADDRESS, &lease, sizeof(lease));

  ip4_addr_t netmask;
  netmask.addr = (uint32_t)eth_subnet;
  dhcps_set_option_info(server, SUBNET_MASK, &netmask, sizeof(netmask));

  ip_addr_t dns;
  eth_primaryDNS.to_ip_addr_t(&dns);
  dhcps_offer_t offer_dns = OFFER_DNS;
  dhcps_set_option_info(server, DOMAIN_NAME_SERVER, &offer_dns, sizeof(offer_dns));
  dhcps_dns_setserver(server, &dns);

  ip4_addr_t server_ip;
  server_ip.addr = (uint32_t)eth_local_IP;

  dhcps_set_new_lease_cb(server, eth_dhcp_lease_cb, NULL);

  if (dhcps_start(server, eth_netif, server_ip) != ERR_OK)
  {
    Serial.println("dhcps_start failed");
    dhcps_delete(server);
    return false;
  }

  eth_dhcp_server = server;
  Serial.printf("Ethernet DHCP server: %s - %s\r\n",
                eth_lease_start.toString().c_str(),
                eth_lease_end.toString().c_str());
  return true;
}

static bool eth_configured = false;
static bool napt_enabled = false;

static void enable_napt_cb(void *arg)
{
  (void)arg;

  if (!eth_configured || !ETH.linkUp())
  {
    Serial.println("ETH link is down; NAPT/DHCP not started");
    return;
  }

#if ENABLE_ETH_DHCP_SERVER
  if (!start_eth_dhcp_lwip())
  {
    Serial.println("Ethernet DHCP server start failed");
  }
#endif

#if ENABLE_NAPT
  netif_set_default(HaLow.netif());
  ip_napt_enable((uint32_t)eth_local_IP, 1);
  napt_enabled = true;
  Serial.println("NAPT enabled: HaLow STA -> Ethernet");
#endif
}

static void disable_napt_cb(void *arg)
{
  (void)arg;

  if (!napt_enabled)
  {
    return;
  }

  ip_napt_enable((uint32_t)eth_local_IP, 0);
  napt_enabled = false;
  Serial.println("NAPT disabled");
}

static void onEthEvent(arduino_event_id_t event, arduino_event_info_t info)
{
  (void)info;

  switch (event)
  {
  case ARDUINO_EVENT_ETH_START:
    Serial.println("ETH started");
    break;

  case ARDUINO_EVENT_ETH_CONNECTED:
    Serial.println("ETH link connected");
    if (tcpip_callback(enable_napt_cb, NULL) != ERR_OK)
    {
      Serial.println("Failed to schedule NAPT enable");
    }
    break;

  case ARDUINO_EVENT_ETH_DISCONNECTED:
  case ARDUINO_EVENT_ETH_STOP:
    Serial.println("ETH link down");
    if (tcpip_callback(disable_napt_cb, NULL) != ERR_OK)
    {
      Serial.println("Failed to schedule NAPT disable");
    }
    break;

  default:
    break;
  }
}

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.printf("\r\nHaLow STA IP mode: %s\r\n",
                HALOW_USE_DHCP ? "DHCP" : "static");

#ifdef HT_RC3268
  pinMode(HALOW_LDO_CTRL, OUTPUT);
  digitalWrite(HALOW_LDO_CTRL, HALOW_LDO_ENABLE);
#endif

#if !HALOW_USE_DHCP
  if (!HaLow.config(halow_local_IP, halow_gateway, halow_subnet,
                    halow_primaryDNS, halow_secondaryDNS))
  {
    Serial.println("HaLow static IP configuration failed");
  }
#endif

  HaLow.init(HALOW_REGION);
  HaLow.begin(HALOW_AP_SSID, HALOW_PASSWORD);

  while (HaLow.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("HaLow STA connected");
  Serial.print("HaLow IP: ");
  Serial.println(HaLow.localIP());
  Serial.print("HaLow gateway: ");
  Serial.println(HaLow.gatewayIP());

  /* Configure W5500 as a static downstream Ethernet interface. */
  Network.onEvent(onEthEvent);

  ethSPI.begin(ETH_SPI_SCK, ETH_SPI_MISO, ETH_SPI_MOSI);

  if (!ETH.begin(ETH_PHY_W5500, ETH_PHY_ADDR, ETH_PHY_CS,
                 ETH_PHY_IRQ, ETH_PHY_RST, ethSPI, 20))
  {
    Serial.println("W5500 Ethernet init failed");
    return;
  }

  delay(50);

  if (!ETH.config(eth_local_IP, eth_gateway, eth_subnet,
                  eth_primaryDNS, eth_secondaryDNS))
  {
    Serial.println("Ethernet static IP configuration failed");
  }
  else
  {
    eth_configured = true;

    Serial.print("ETH IP: ");
    Serial.println(ETH.localIP());

  }

  if (ETH.linkUp())
  {
    if (tcpip_callback(enable_napt_cb, NULL) != ERR_OK)
    {
      Serial.println("Failed to schedule NAPT enable");
    }
  }
  else
  {
    Serial.println("ETH link is down; NAPT/DHCP will start when link is up");
  }
}

void loop()
{
  static uint32_t last_report = 0;
  uint32_t now = millis();

  if (now - last_report >= 30000)
  {
    last_report = now;
    Serial.printf("[%u][NAPT] HaLow=%s ETH=%s\r\n",millis()/1000,
                  HaLow.localIP().toString().c_str(),
                  ETH.localIP().toString().c_str());
  }

  delay(100);
}
