#include "HaLow.h"

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
        case ARDUINO_HALOW_EVENT_STA_GOT_IP:
            Serial.println("HALOW_EVENT_STA_GOT_IP");
            Serial.print("IP:");
            Serial.println(HaLow.localIP());
            Serial.print("NetMask:");
            Serial.println(HaLow.subnetMask());
            Serial.print("Gateway:");
            Serial.println(HaLow.gatewayIP());
            break;
        case ARDUINO_HALOW_EVENT_STA_LOST_IP:
            Serial.println("ARDUINO_HALOW_EVENT_STA_LOST_IP");
            break;
        default: break;
    }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  HaLow.onEvent(HaLowEvent);
  HaLow.init("US");
  HaLow.begin("MorseMicro2","123456789");
  Serial.println("Wait for Halow... ");
}

void loop() {
  delay(1000);
}
