#include "HaLow.h"

#define HALOW_REGION     "US"
#define HALOW_AP_SSID    "HaLow-AP"
#define HALOW_PASSWORD   "heltec.org"

void HaLowEvent(HaLowEvent_t event)
{
    Serial.printf("[HaLow-event] event: %d\r\n", event);

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
  delay(1000);
}
