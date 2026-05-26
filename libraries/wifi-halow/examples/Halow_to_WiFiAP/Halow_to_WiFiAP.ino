#include <HaLow.h>
#include "WiFi.h"

#include <Arduino.h>
#include <WiFi.h>
#include <esp_netif.h>
#include <esp_netif_net_stack.h>
#include <lwip/netif.h>
#include <dhcpserver/dhcpserver.h>
#include "mmwlan.h"
#include "mmipal.h"

const char* ssid     = "HT-H7608-BB2E";
const char* password = "heltec.org";
const char* host     = "www.baidu.com";
const char* url      = "/index.html";
IPAddress hostIP;
#define TAG "TEST"

#define AP_SSID         "NaptTest"
#define AP_PASSWORD     "12345678"
#define UPSTREAM_DNS    "8.8.8.8"   // your HaLow side's router or 8.8.8.8
#define AP_CHANNEL      6


static bool softap_with_napt() {
    if (!WiFi.mode(WIFI_AP)) {
        Serial.printf("[%s] WiFi.mode(WIFI_AP) failed\n", TAG);
        return false;
    }
    delay(10);
    esp_netif_t* ap_netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    if (ap_netif != nullptr) {
        Serial.println("set dns");
        dhcps_offer_t offer_dns = OFFER_DNS;
        esp_netif_dhcps_option(ap_netif, ESP_NETIF_OP_SET,
                               ESP_NETIF_DOMAIN_NAME_SERVER,
                               &offer_dns, sizeof(offer_dns));

        esp_netif_dns_info_t dns_info = {};
        dns_info.ip.u_addr.ip4.addr = ipaddr_addr(UPSTREAM_DNS);
        dns_info.ip.type = ESP_IPADDR_TYPE_V4;
        esp_netif_set_dns_info(ap_netif, ESP_NETIF_DNS_MAIN, &dns_info);
    }

    delay(10);
    if (!WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL)) {
        Serial.printf("[%s] softAP failed\n", TAG);
        return false; 
    }
    Serial.printf("[%s] softAP up: SSID=%s IP=%s\n",
                  TAG, AP_SSID, WiFi.softAPIP().toString().c_str());

    /* Enable NAPT on the AP netif */
    delay(10);
    esp_err_t err = esp_netif_napt_enable(ap_netif);
    if (err != ESP_OK) {
        Serial.printf("[%s] esp_netif_napt_enable failed: %s (0x%x)\n",
                      TAG, esp_err_to_name(err), err);
        return false;
    }
    Serial.printf("[%s] NAPT enabled on AP netif\n", TAG);

    /* HaLow as default route */
    delay(10);
    netif_set_default(HaLow.netif());
    Serial.printf("[%s] default route set to HaLow netif\n", TAG);

    return true;
}



void setup()
{
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  
#ifdef HT_RC3268
  //enable WiFiHalow LDO
  pinMode(HALOW_LDO_CTRL,OUTPUT);
  digitalWrite(HALOW_LDO_CTRL,HALOW_LDO_ENABLE);
#endif

  HaLow.init("US");
  
  HaLow.begin(ssid, password);

  while (HaLow.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("HaLow connected!");
  Serial.print("IP address: ");
  Serial.println(HaLow.localIP());
  Serial.print("Mac Address: ");
  Serial.println(HaLow.macAddress());
  Serial.print("Subnet Mask: ");
  Serial.println(HaLow.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(HaLow.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(HaLow.dnsIP());

  delay(20);
  softap_with_napt();  
}


void loop()
{
  delay(5000);

  Serial.print("connecting to ");
  Serial.println(host);

  // Use HalowClient class to create TCP connections
  HalowClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");

  Serial.printf("[time %u s] halow rssi %d\r\n",millis()/1000,HaLow.RSSI());
}
