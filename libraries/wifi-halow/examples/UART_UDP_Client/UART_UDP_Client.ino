#include <HaLow.h>
#include <WiFiUdp.h>

const char* ssid     = "HT-HR01-FE8F";
const char* password = "heltec.org";

#define SERVER_ADDR "192.168.0.76"
#define SERVER_PORT 10000

#define LOCAL_PORT 10001

WiFiUDP Udp;

bool halowconnected=false;
bool udpstarted = false;
int rx_index = 0;
uint8_t rx_buff[2048];
uint8_t tx_buff[2048];

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
            halowconnected=false;
            break;
        case ARDUINO_HALOW_EVENT_STA_GOT_IP:
            Serial.println("HALOW_EVENT_STA_GOT_IP");
            Serial.print("IP:");
            Serial.println(HaLow.localIP());
            Serial.print("NetMask:");
            Serial.println(HaLow.subnetMask());
            Serial.print("Gateway:");
            Serial.println(HaLow.gatewayIP());
            halowconnected=true;
            break;
        case ARDUINO_HALOW_EVENT_STA_LOST_IP:
            Serial.println("ARDUINO_HALOW_EVENT_STA_LOST_IP");
            halowconnected=false;
            break;
        default: break;
    }
}


void setup()
{
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  HaLow.onEvent(HaLowEvent);
  HaLow.init("US");

  HaLow.begin(ssid, password);

}

void loop()
{
  if(halowconnected)
  {
    if(udpstarted==false)
    {
      udpstarted=true;
      Serial.println("UDP begin");
      Udp.begin(LOCAL_PORT); 
    }
    else
    {
      if(Serial.available())
      {
        int i=0;
        while(Serial.available())
        {
          tx_buff[i++]=Serial.read(); 
          if(Serial.available()==0)
          {
            delay(10);
          }
        }
        Udp.beginPacket(SERVER_ADDR, SERVER_PORT);
        Udp.write(tx_buff,i);
        Udp.endPacket(); 
      }

      rx_index = Udp.parsePacket();
      if(rx_index)
      {
        Udp.read(rx_buff, rx_index);
        Serial.write(rx_buff,rx_index);
        Serial.flush();
      }
    }
  }
}
