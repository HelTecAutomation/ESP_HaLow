#include <HaLow.h>

const char* ssid     = "HT-HR01-FE8F";
const char* password = "heltec.org";

#define SERVER_ADDR "192.168.0.76"
#define SERVER_PORT 10000

#define LOCAL_PORT 10001

HalowClient client(LOCAL_PORT);

uint32_t t=0;
int rx_index = 0;
uint8_t rx_buff[2048];
uint8_t tx_buff[2048];
bool halowconnected=false;

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
    if(client.connected()==0)
    {
      client.connect(SERVER_ADDR,SERVER_PORT);
      delay(2000);
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
        client.write(tx_buff,i);
      }

      if(client.available())
      {
        rx_index = 0;
        while(client.available())
        {
            rx_buff[rx_index++]=client.read();
        }
        Serial.write(rx_buff,rx_index);
        Serial.flush();
      }
    }
  }
}
