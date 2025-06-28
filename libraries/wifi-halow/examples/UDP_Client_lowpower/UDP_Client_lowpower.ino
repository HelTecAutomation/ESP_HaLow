/*
This is an lowpower example that cycle wakeup and send UDP message.
And it can auto wakeup when received UDP message.
*/
#include <HaLow.h>
#include <WiFiUdp.h>
#include <driver/gpio.h>

const char* ssid     = "HT-HR01-FE8F";
const char* password = "heltec.org";

#define SERVER_ADDR "192.168.0.76"
#define SERVER_PORT 10000

#define LOCAL_PORT 10001
#define SEND_UDP_INTERVAL 10000 // 10s
WiFiUDP Udp;
bool halowconnected=false;
bool udpstarted = false;
int rx_index = 0;
uint8_t rx_buff[2048];
uint8_t tx_buff[2048];
uint32_t last_send_udp_time=0;

#define TEST_UDP_MESSAGE "cycle udp message\r\n"
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


void  halow_esp_sleep()
{
  if(!udpstarted)
  {
    return;
  }

  if(digitalRead(CONFIG_MM_BUSY)==0 && digitalRead(CONFIG_MM_WAKE)==0)
  {
    esp_sleep_enable_ext1_wakeup(1<<CONFIG_MM_BUSY,ESP_EXT1_WAKEUP_ANY_HIGH);
    uint32_t sleeptime= last_send_udp_time+SEND_UDP_INTERVAL-millis();
    if(sleeptime>SEND_UDP_INTERVAL)
    {
      sleeptime=SEND_UDP_INTERVAL;
    }
    esp_sleep_enable_timer_wakeup( sleeptime * 1000 );

    Serial.flush();
    esp_light_sleep_start();
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
      last_send_udp_time=millis();
      Serial.println("UDP begin");
      Udp.begin(LOCAL_PORT); 
    }
    else
    {
      rx_index = Udp.parsePacket();
      if(rx_index)
      {
        Udp.read(rx_buff, rx_index);
        Serial.write(rx_buff,rx_index);
        Serial.flush();
      }
      halow_esp_sleep();

      if( millis()-last_send_udp_time >= SEND_UDP_INTERVAL  )
      {
        Serial.println("send udp message");
        Udp.beginPacket(SERVER_ADDR, SERVER_PORT);
        Udp.write((uint8_t *)TEST_UDP_MESSAGE,strlen(TEST_UDP_MESSAGE));
        Udp.endPacket();
        delay(1);//delay avoid into sleep before data transmit to halow modue
        last_send_udp_time+=SEND_UDP_INTERVAL;
      }

    }
  }
}
