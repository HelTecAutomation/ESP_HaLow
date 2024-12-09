//library need : https://github.com/marvinroger/async-mqtt-client

#include "HaLow.h"
#include <AsyncMqttClient.h>
#define MQTT_HOST IPAddress(43,154,113,203)
#define MQTT_PORT 1883
IPAddress ServerIP;
bool halow_connected=false;
void HaLowEvent(HaLowEvent_t event)
{
    Serial.printf("[HaLow-event] event: %d\r\n", event);

    switch (event) {
        case ARDUINO_HALOW_EVENT_STA_CONNECTING: 
            Serial.println("ARDUINO_HALOW_EVENT_STA_CONNECTING");
            break;
        case ARDUINO_HALOW_EVENT_STA_CONNECTED:
            Serial.println("ARDUINO_HALOW_EVENT_STA_CONNECTED");
            break;
        case ARDUINO_HALOW_EVENT_STA_DISCONNECTED:
            Serial.println("ARDUINO_HALOW_EVENT_STA_DISCONNECTED");
            halow_connected=false;
            break;
        case ARDUINO_HALOW_EVENT_STA_GOT_IP:
            Serial.println("HALOW_EVENT_STA_GOT_IP");
            Serial.print("IP:");
            Serial.println(HaLow.localIP());
            Serial.print("NetMask:");
            Serial.println(HaLow.subnetMask());
            Serial.print("Gateway:");
            Serial.println(HaLow.gatewayIP());

            Serial.println(HaLow.hostByName("minerback.heltec.cn", ServerIP));
            Serial.println(ServerIP);
            Serial.println(HaLow.BSSIDstr());
            halow_connected=true;
            break;
        case ARDUINO_HALOW_EVENT_STA_LOST_IP:
            Serial.println("ARDUINO_HALOW_EVENT_STA_LOST_IP");
            Serial.print("IP:");
            Serial.println(HaLow.localIP());
            Serial.print("NetMask:");
            Serial.println(HaLow.subnetMask());
            Serial.print("Gateway:");
            Serial.println(HaLow.gatewayIP());
            halow_connected=false;
            break;
        default: break;
    }
}


AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

bool mqttconnected=false;

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}


void onMqttConnect(bool sessionPresent) {
  mqttconnected=true;
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  uint16_t packetIdSub = mqttClient.subscribe("gettime", 2);
  Serial.print("Subscribing at QoS 2, packetId: ");
  Serial.println(packetIdSub);
  mqttClient.publish("test/lol", 0, true, "test 1");
  Serial.println("Publishing at QoS 0");
  uint16_t packetIdPub1 = mqttClient.publish("test/lol", 1, true, "test 2");
  Serial.print("Publishing at QoS 1, packetId: ");
  Serial.println(packetIdPub1);
  uint16_t packetIdPub2 = mqttClient.publish("test/lol", 2, true, "test 3");
  Serial.print("Publishing at QoS 2, packetId: ");
  Serial.println(packetIdPub2);
}


void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  mqttconnected=false;
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
  Serial.print("  payload: ");
  Serial.write(payload,len);
  Serial.println();
  char tmp[50];
  sprintf(tmp,"%d",millis()/1000);
  mqttClient.publish("test/lol", 0, true, tmp);
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}


void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.printf("\n\nMorse Iperf Demo (Built " __DATE__ " " __TIME__ ")\n\n");
    Serial.println(HaLow.macAddress());
    uint8_t mac[6];
    HaLow.macAddress(mac);
    Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    HaLow.onEvent(HaLowEvent);
    HaLow.config(IPAddress(192,168,0,208),IPAddress(192,168,0,1),IPAddress(255,255,255,0));
    HaLow.setDNS(IPAddress(114,114,114,114),IPAddress(192,168,0,1));
    HaLow.init();
    HaLow.begin("MorseMicro2","123456789");
    Serial.println(HaLow.macAddress());
    HaLow.macAddress(mac);
    Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    /* Initialize and connect to WiFi, blocks till connected */
//    app_wlan_init();
//    uint8_t  mac_test[6]={0,1,2,3,4,5};
//    mmhal_read_mac_addr(mac_test);
//    Serial.printf("%02X %02X %02X %02X %02X %02X \r\n",mac_test[0],mac_test[1],mac_test[2],mac_test[3],mac_test[4],mac_test[5]);
//    app_wlan_start();
//    mmhal_read_mac_addr(mac_test);
//    Serial.printf("%02X %02X %02X %02X %02X %02X \r\n",mac_test[0],mac_test[1],mac_test[2],mac_test[3],mac_test[4],mac_test[5]);
    
//    WiFi.mode(WIFI_AP);
//    WiFi.softAP("ESP32-Halow-test","12345678");

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.printf("[time %u] rssi %d\r\n",millis()/1000,HaLow.RSSI());
//  Serial.println(HaLow.broadcastIP());
//  Serial.println(HaLow.networkID());
//  Serial.println(HaLow.subnetCIDR());
//  Serial.println(HaLow.dnsIP(0));
//  Serial.println(HaLow.dnsIP(1));
  delay(5000);
  if(mqttconnected==false && halow_connected)
  {
     Serial.println("connect to mqtt");
     connectToMqtt();
  }
}
