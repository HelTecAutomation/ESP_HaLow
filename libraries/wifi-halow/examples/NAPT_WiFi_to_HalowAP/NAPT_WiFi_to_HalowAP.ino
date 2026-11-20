/*
 * Important Notes
 * 1. This example runs a HaLow AP and connects the ESP32 to a WiFi AP as STA.
 * 2. NAPT is enabled after the WiFi uplink obtains an IP, forwarding traffic
 *    between the HaLow AP subnet and the WiFi uplink.
 * 3. ENABLE_DHCP_SERVER = 1 starts the embedded DHCP server for HaLow STAs.
 *    Set it to 0 if the HaLow STAs use static IP addresses.
 */
#include <HaLow.h>
#include "WiFi.h"
#include "lwip/tcpip.h"
#include "mmipal.h"

// Please read REGION_SW_CHANNEL_REF. h to obtain the relationship between region, bandwidth, and channel
#define HALOW_REGION     "US"
#define HALOW_CHANNEL     28
#define HALOW_AP_SSID    "HaLow-AP-137"
#define HALOW_PASSWORD   "12345678"

IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

const char* wifi_ssid = "HT-HD02-7F82";
const char* wifi_password = "heltec.org";

// Set to 0 if HaLow STAs use static IP addresses.
#ifndef ENABLE_DHCP_SERVER
#define ENABLE_DHCP_SERVER 1
#endif

// DHCP lease range for HaLow STAs.
IPAddress dhcp_lease_start(192, 168, 1, 100);
IPAddress dhcp_lease_end(192, 168, 1, 150);

#if ENABLE_DHCP_SERVER
static void onDhcpLease(IPAddress client_ip, const uint8_t client_mac[6])
{
  Serial.printf("DHCP lease %s -> %02X:%02X:%02X:%02X:%02X:%02X\n",
                client_ip.toString().c_str(),
                client_mac[0], client_mac[1], client_mac[2],
                client_mac[3], client_mac[4], client_mac[5]);
}
#endif

/*
 * NAPT must be enabled from the lwIP TCP/IP thread, after the WiFi uplink
 * has an IP address.
 */
static void enableHalowNapt(void *arg)
{
  (void)arg;
  ip_napt_enable((uint32_t)local_IP, 1);
  Serial.println("NAPT enabled on HaLow AP");
}

// WARNING: This function is called from a separate FreeRTOS task (thread)!
void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);

  switch (event) {
    case ARDUINO_EVENT_WIFI_READY:               Serial.println("WiFi interface ready"); break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:           Serial.println("Completed scan for access points"); break;
    case ARDUINO_EVENT_WIFI_STA_START:           Serial.println("WiFi client started"); break;
    case ARDUINO_EVENT_WIFI_STA_STOP:            Serial.println("WiFi clients stopped"); break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:       Serial.println("Connected to access point"); break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:    Serial.println("Disconnected from WiFi access point"); break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE: Serial.println("Authentication mode of access point has changed"); break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("Obtained IP address: ");
      Serial.println(WiFi.localIP());
      break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:        Serial.println("Lost IP address and IP address is reset to 0"); break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:          Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_FAILED:           Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:          Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_PIN:              Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode"); break;
    case ARDUINO_EVENT_WIFI_AP_START:           Serial.println("WiFi access point started"); break;
    case ARDUINO_EVENT_WIFI_AP_STOP:            Serial.println("WiFi access point  stopped"); break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:    Serial.println("Client connected"); break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED: Serial.println("Client disconnected"); break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:   Serial.println("Assigned IP address to client"); break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:  Serial.println("Received probe request"); break;
    case ARDUINO_EVENT_WIFI_AP_GOT_IP6:         Serial.println("AP IPv6 is preferred"); break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:        Serial.println("STA IPv6 is preferred"); break;
    case ARDUINO_EVENT_ETH_GOT_IP6:             Serial.println("Ethernet IPv6 is preferred"); break;
    case ARDUINO_EVENT_ETH_START:               Serial.println("Ethernet started"); break;
    case ARDUINO_EVENT_ETH_STOP:                Serial.println("Ethernet stopped"); break;
    case ARDUINO_EVENT_ETH_CONNECTED:           Serial.println("Ethernet connected"); break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:        Serial.println("Ethernet disconnected"); break;
    case ARDUINO_EVENT_ETH_GOT_IP:              Serial.println("Obtained IP address"); break;
    default:                                    break;
  }
}

// WARNING: This function is called from a separate FreeRTOS task (thread)!
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));

  if (tcpip_callback(enableHalowNapt, NULL) != ERR_OK) {
    Serial.println("Failed to schedule NAPT enable");
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

#ifdef HT_RC3268
  //enable WiFiHalow LDO
  pinMode(HALOW_LDO_CTRL,OUTPUT);
  digitalWrite(HALOW_LDO_CTRL,HALOW_LDO_ENABLE);
#endif

  if (!HaLow.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  HaLow.init(HALOW_REGION);
  HaLow.AP(HALOW_AP_SSID,HALOW_PASSWORD,HALOW_CHANNEL);
  while (HaLow.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("HaLow AP Started!");
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
  if (HaLow.enableDhcpServer(dhcp_lease_start, dhcp_lease_end)) {
    Serial.println("DHCP server started");
  } else {
    Serial.println("DHCP server start failed");
  }
#else
  Serial.println("DHCP server disabled; HaLow STAs need static IP addresses.");
#endif

  WiFi.onEvent(WiFiEvent);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFiEventId_t eventID = WiFi.onEvent(
    [](WiFiEvent_t event, WiFiEventInfo_t info) {
      Serial.print("WiFi lost connection. Reason: ");
      Serial.println(info.wifi_sta_disconnected.reason);
    },
    WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED
  );
  WiFi.begin(wifi_ssid, wifi_password);

  Serial.println();
  Serial.println("Wait for WiFi... ");
}

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

static void printHalowStations()
{
  struct mmwlan_ap_sta_status sta_list[HALOW_AP_MAX_STAS];
  uint8_t count = HaLow.getStationList(sta_list, HALOW_AP_MAX_STAS);

  Serial.printf("[STA] connected count: %u\r\n", count);
  for (uint8_t i = 0; i < count; i++)
  {
    struct mmipal_sta_rssi rssi;
    enum mmipal_status status = mmipal_get_sta_rssi(sta_list[i].mac_addr, &rssi);
    uint32_t last_seen = mmwlan_ap_get_sta_last_seen_ms(sta_list[i].mac_addr);

    Serial.printf("[STA %u] %02X:%02X:%02X:%02X:%02X:%02X state=%s aid=%u last_80211=%lu ms",
                  i,
                  sta_list[i].mac_addr[0], sta_list[i].mac_addr[1],
                  sta_list[i].mac_addr[2], sta_list[i].mac_addr[3],
                  sta_list[i].mac_addr[4], sta_list[i].mac_addr[5],
                  staStateName(sta_list[i].state),
                  (unsigned)sta_list[i].aid,
                  (unsigned long)last_seen);

    if (status == MMIPAL_SUCCESS)
    {
      Serial.printf(" rssi=%d dBm noise=%d dBm bw=%u MHz freq=%lu kHz last_data_rx=%lu ms\r\n",
                    rssi.rssi, rssi.noise_dbm, rssi.bw_mhz,
                    (unsigned long)rssi.freq_100khz * 100UL,
                    (unsigned long)rssi.last_rx_ms);
    }
    else
    {
      Serial.println(" rssi/noise: no recent data");
    }
  }
}

void loop() {
  static uint32_t last_report = 0;
  uint32_t now = millis();

  if (now - last_report >= 30000) {
    last_report = now;
    printHalowStations();
  }

  delay(100);
}

