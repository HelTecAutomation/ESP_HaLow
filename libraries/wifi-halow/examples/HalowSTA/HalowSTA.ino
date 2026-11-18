#include "HaLow.h"
#include <WiFiUdp.h>

#define HALOW_REGION     "US"
#define HALOW_AP_SSID    "HaLow-AP"
#define HALOW_PASSWORD   "heltec.org"

#define GATEWAY_UDP_PORT 10000
#define LOCAL_UDP_PORT   10001
#define SEND_INTERVAL_MS 10000

WiFiUDP Udp;
bool udp_started = false;
uint32_t udp_send_count = 0;

void HaLowEvent(HaLowEvent_t event)
{
    Serial.printf("[HaLow-event] event: %d - %s\r\n", event, HaLow.eventName(event));

    switch (event) {
        case ARDUINO_HALOW_EVENT_SCAN_DONE: 
            Serial.println("ARDUINO_HALOW_EVENT_SCAN_DONE");
            break;
        case ARDUINO_HALOW_EVENT_STA_CONNECTING: 
            Serial.println("ARDUINO_HALOW_EVENT_STA_CONNECTING");
            break;
        case ARDUINO_HALOW_EVENT_STA_CONNECTED:
            Serial.println("ARDUINO_HALOW_EVENT_STA_CONNECTED");
            break;
        case ARDUINO_HALOW_EVENT_STA_DISCONNECTED:
            Serial.println("ARDUINO_HALOW_EVENT_STA_DISCONNECTED");
            break;
        case ARDUINO_HALOW_EVENT_GOT_IP:
            Serial.println("HALOW_EVENT_GOT_IP");
            Serial.print("IP:");
            Serial.println(HaLow.localIP());
            Serial.print("NetMask:");
            Serial.println(HaLow.subnetMask());
            Serial.print("Gateway:");
            Serial.println(HaLow.gatewayIP());
            break;
        case ARDUINO_HALOW_EVENT_LOST_IP:
            Serial.println("ARDUINO_HALOW_EVENT_LOST_IP");
            break;
        default: break;
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

  HaLow.onEvent(HaLowEvent);
  HaLow.init(HALOW_REGION);
  HaLow.begin(HALOW_AP_SSID, HALOW_PASSWORD);
  Serial.println("Wait for Halow... ");
}

void loop() {
  static uint32_t last_report_ms = 0;
  static uint32_t last_send_ms = 0;
  uint32_t now = millis();

  if (HaLow.status() != WL_CONNECTED) {
    if (udp_started) {
      Udp.stop();
      udp_started = false;
    }
    if (now - last_report_ms >= 1000) {
      last_report_ms = now;
      Serial.println("[HaLow-link] not connected");
    }
    delay(100);
    return;
  }

  if (!udp_started) {
    if (Udp.begin(LOCAL_UDP_PORT)) {
      udp_started = true;
      last_send_ms = now;
      Serial.printf("[HaLow-udp] local port %u\r\n", LOCAL_UDP_PORT);
    } else {
      Serial.println("[HaLow-udp] begin failed");
    }
  }

  uint8_t bssid[6];
  struct mmipal_sta_rssi signal;
  bool have_signal = false;

  if (HaLow.BSSID(bssid) != NULL &&
      mmipal_get_sta_rssi(bssid, &signal) == MMIPAL_SUCCESS) {
    have_signal = true;
  }

  int8_t rssi = have_signal ? signal.rssi : HaLow.RSSI();

  if (now - last_report_ms >= 10000) {
    last_report_ms = now;

    Serial.printf("[HaLow-link] RSSI: %d dBm", rssi);
    if (have_signal && signal.noise_dbm > -128 && signal.noise_dbm < 0) {
      Serial.printf(" Noise: %d dBm", signal.noise_dbm);
    } else {
      Serial.print(" Noise: n/a");
    }
    if (have_signal && signal.bw_mhz > 0) {
      Serial.printf(" Bandwidth: %u MHz", signal.bw_mhz);
    } else {
      Serial.print(" Bandwidth: n/a");
    }
    if (have_signal && signal.freq_100khz > 0) {
      Serial.printf(" Frequency: %u.%u MHz",
                    (unsigned)(signal.freq_100khz / 10),
                    (unsigned)(signal.freq_100khz % 10));
    } else {
      Serial.print(" Frequency: n/a");
    }
    Serial.println();
  }

  if (now - last_send_ms >= SEND_INTERVAL_MS) {
    last_send_ms = now;

    char message[128];
    int len = snprintf(message, sizeof(message),
                       "HalowSTA #%lu rssi=%d noise=%d bw=%u freq=%u.%uMHz",
                       (unsigned long)(udp_send_count + 1),
                       rssi,
                       (have_signal && signal.noise_dbm > -128 && signal.noise_dbm < 0)
                           ? signal.noise_dbm : 0,
                       (unsigned)(have_signal ? signal.bw_mhz : 0),
                       (unsigned)(have_signal ? signal.freq_100khz / 10 : 0),
                       (unsigned)(have_signal ? signal.freq_100khz % 10 : 0));

    if (len > 0) {
      if (Udp.beginPacket(HaLow.gatewayIP(), GATEWAY_UDP_PORT) == 1) {
        Udp.write((const uint8_t *)message, len);
        if (Udp.endPacket() == 1) {
          udp_send_count++;
          Serial.printf("[HaLow-udp] sent #%lu to %s:%u (%d bytes)\r\n",
                        (unsigned long)udp_send_count,
                        HaLow.gatewayIP().toString().c_str(),
                        GATEWAY_UDP_PORT, len);
        } else {
          Serial.println("[HaLow-udp] send failed");
        }
      } else {
        Serial.println("[HaLow-udp] beginPacket failed");
      }
    }
  }

  if (udp_started) {
    int packetSize = Udp.parsePacket();
    if (packetSize > 0) {
      IPAddress remoteIp = Udp.remoteIP();
      uint16_t remotePort = Udp.remotePort();

      char reply[256];
      int len = Udp.read(reply, sizeof(reply) - 1);
      if (len < 0) {
        len = 0;
      }
      reply[len] = '\0';

      Serial.printf("[HaLow-udp] RX from %s:%u (%d bytes): %s\r\n",
                    remoteIp.toString().c_str(), remotePort, len, reply);
    }
  }

  delay(100);
}
