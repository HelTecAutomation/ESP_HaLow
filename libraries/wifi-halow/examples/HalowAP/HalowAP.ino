/*
 * Important Notes
 * 1. ENABLE_DHCP_SERVER = 1: the AP starts an embedded DHCP server (implemented
 *    in HalowAPClass) and leases addresses from dhcp_lease_start to
 *    dhcp_lease_end to HaLow STAs.
 * 2. ENABLE_DHCP_SERVER = 0: no DHCP server; every HaLow STA must use a static
 *    IP address in the same subnet (see HalowClientStaticIP).
 */
#include <HaLow.h>
#include "WiFi.h"
#include <WiFiUdp.h>

// Please read REGION_SW_CHANNEL_REF.h to obtain the relationship between region, bandwidth,  channel and freq.
#define HALOW_REGION     "US"
#define HALOW_CHANNEL     28
// 0 = let the library use the bandwidth of the selected channel entry.
#define HALOW_BANDWIDTH   8
#define HALOW_MAX_STAS    4
#define HALOW_AP_SSID    "HaLow-AP"
#define HALOW_PASSWORD   "heltec.org"

#define UDP_LISTEN_PORT  10000

WiFiUDP Udp;

// Set to 0 to disable the embedded DHCP server (can also be overridden with -D).
#ifndef ENABLE_DHCP_SERVER
#define ENABLE_DHCP_SERVER 1
#endif

// DHCP lease range (only used when ENABLE_DHCP_SERVER is 1).
IPAddress dhcp_lease_start(192, 168, 1, 100);
IPAddress dhcp_lease_end(192, 168, 1, 150);

IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); 
IPAddress secondaryDNS(8, 8, 4, 4); 

#if ENABLE_DHCP_SERVER
static void onDhcpLease(IPAddress client_ip, const uint8_t client_mac[6])
{
  Serial.printf("DHCP lease %s -> %02X:%02X:%02X:%02X:%02X:%02X\n",
                client_ip.toString().c_str(),
                client_mac[0], client_mac[1], client_mac[2],
                client_mac[3], client_mac[4], client_mac[5]);
}
#endif

static const char *staStateName(enum mmwlan_ap_sta_state state)
{
  switch (state)
  {
    case MMWLAN_AP_STA_ASSOCIATED:
      return "ASSOCIATED";
    case MMWLAN_AP_STA_AUTHORIZED:
      return "AUTHORIZED";
    default:
      return "UNKNOWN";
  }
}

static void printStaStatus()
{
  // RSSI/noise are updated when data frames are received from the STA.
  struct mmwlan_ap_sta_status sta_list[MMWLAN_AP_MAX_STAS_LIMIT];
  uint8_t count = HaLow.getStationList(sta_list, MMWLAN_AP_MAX_STAS_LIMIT);

  Serial.printf("[STA] connected count: %u\n", count);
  for (uint8_t i = 0; i < count; i++)
  {
    struct mmipal_sta_rssi rssi;
    enum mmipal_status status = mmipal_get_sta_rssi(sta_list[i].mac_addr, &rssi);

    Serial.printf("[STA %u] %02X:%02X:%02X:%02X:%02X:%02X state=%s aid=%u",
                  i,
                  sta_list[i].mac_addr[0], sta_list[i].mac_addr[1],
                  sta_list[i].mac_addr[2], sta_list[i].mac_addr[3],
                  sta_list[i].mac_addr[4], sta_list[i].mac_addr[5],
                  staStateName(sta_list[i].state),
                  (unsigned)sta_list[i].aid);

    if (status == MMIPAL_SUCCESS)
    {
      Serial.printf(" rssi=%d dBm noise=%d dBm bw=%u MHz freq=%lu kHz last_rx=%lu ms\n",
                    rssi.rssi, rssi.noise_dbm, rssi.bw_mhz,
                    (unsigned long)rssi.freq_100khz * 100UL,
                    (unsigned long)rssi.last_rx_ms);
    }
    else
    {
      Serial.println(" rssi/noise: no recent data frame");
    }
  }
}

static void handleUdp()
{
  int packetSize = Udp.parsePacket();
  if (packetSize <= 0)
  {
    return;
  }

  IPAddress remoteIp = Udp.remoteIP();
  uint16_t remotePort = Udp.remotePort();

  char buffer[256];
  int len = Udp.read(buffer, sizeof(buffer) - 1);
  if (len < 0)
  {
    len = 0;
  }
  buffer[len] = '\0';

  Serial.printf("[HaLow-udp] RX from %s:%u (%d bytes): %s\r\n",
                remoteIp.toString().c_str(), remotePort, len, buffer);

  char reply[320];
  int replyLen = snprintf(reply, sizeof(reply), "HaLow-AP ACK: %s", buffer);
  if (replyLen <= 0)
  {
    return;
  }
  if (replyLen >= (int)sizeof(reply))
  {
    replyLen = sizeof(reply) - 1;
  }

  if (Udp.beginPacket(remoteIp, remotePort) == 1)
  {
    Udp.write((const uint8_t *)reply, replyLen);
    if (Udp.endPacket() == 1)
    {
      Serial.printf("[HaLow-udp] TX to %s:%u (%d bytes)\r\n",
                    remoteIp.toString().c_str(), remotePort, replyLen);
    }
    else
    {
      Serial.println("[HaLow-udp] reply send failed");
    }
  }
  else
  {
    Serial.println("[HaLow-udp] reply beginPacket failed");
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  if (!HaLow.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

#ifdef HT_RC3268
  //enable WiFiHalow LDO
  pinMode(HALOW_LDO_CTRL,OUTPUT);
  digitalWrite(HALOW_LDO_CTRL,HALOW_LDO_ENABLE);
#endif

  // Please read REGION_SW_CHANNEL_REF.h for the region/bandwidth/channel mapping.
  HaLow.init(HALOW_REGION);
  HaLow.AP(HALOW_AP_SSID, HALOW_PASSWORD, HALOW_CHANNEL,
           HALOW_BANDWIDTH, HALOW_MAX_STAS);
  while (HaLow.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("HaLow AP Started!");
  Serial.print("Channel: ");
  Serial.println(HaLow.getChannel());
  Serial.print("Op class: ");
  Serial.println(HaLow.getOpClass());
  Serial.print("Bandwidth: ");
  Serial.print(HaLow.getBandwidth());
  Serial.println(" MHz");
  Serial.print("Frequency: ");
  Serial.print(HaLow.getFrequency() / 1000000.0, 3);
  Serial.println(" MHz");
  Serial.print("IP address: ");
  Serial.println(HaLow.localIP());
  Serial.print("Subnet Mask: ");
  Serial.println(HaLow.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(HaLow.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(HaLow.dnsIP());
  Serial.print("Mac Address: ");
  Serial.println(HaLow.macAddress());

#if ENABLE_DHCP_SERVER
  HaLow.setDhcpLeaseCallback(onDhcpLease);
  if (HaLow.enableDhcpServer(dhcp_lease_start, dhcp_lease_end))
  {
    Serial.println("DHCP server started");
  }
  else
  {
    Serial.println("DHCP server start failed");
  }
#else
  Serial.println("DHCP server disabled; clients must use static IP addresses.");
#endif

  if (Udp.begin(UDP_LISTEN_PORT))
  {
    Serial.printf("[HaLow-udp] listening on port %u\r\n", UDP_LISTEN_PORT);
  }
  else
  {
    Serial.println("[HaLow-udp] begin failed");
  }
}

void loop() {
  static uint32_t last_report_ms = 0;
  uint32_t now = millis();

  handleUdp();

  if (now - last_report_ms >= 10000) {
    last_report_ms = now;
    printStaStatus();
  }

  delay(100);
}
