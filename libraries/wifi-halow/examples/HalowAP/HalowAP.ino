/*
 * Important Notes
 * 1. The ESP32 HaLow AP does not come with an embedded DHCP server.
 * 2. Every HaLow STA client associated with this HaLow AP must be assigned a static IP address from the same subnet to establish communication.
 * 3. Relevant reference sample projects:
 *    - HalowClientStaticIP
 *    - NAPT_HalowSTA_STATIC_to_WiFiAP
 */
#include <HaLow.h>
#include "WiFi.h"

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

  //Please read REGION_SW_CHANNEL_REF. h to obtain the relationship between region, bandwidth, and channel
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

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
}
