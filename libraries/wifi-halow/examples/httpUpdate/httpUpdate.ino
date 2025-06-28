/**
   httpUpdate.ino

    Created on: 27.11.2015

*/

#include <HaLow.h>

#include "config.h"
#include "esp_sleep.h"
#include "driver/uart.h"
#include "Arduino.h"
#include "WiFi.h"
#include <HalowHTTPClient.h>
#include <HalowHTTPUpdate.h>



void update_started() {
  Serial.print("CALLBACK:  HTTP update process started\r\n");
}

void update_finished() {
  Serial.print("CALLBACK:  HTTP update process finished\r\n");
  Serial.flush();
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\r\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\r\n", err);
}


void setup() {

  Serial.begin(115200);
  HaLow.init("US");
  HaLow.begin("HT-HR01-FE8F", "heltec.org");
  Serial.print("Halow connectting");
  while(HaLow.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
}

void loop() {
// wait for WiFi connection
  if(HaLow.status() == WL_CONNECTED) {

    HalowClient client;

    httpUpdate.onStart(update_started);
    httpUpdate.onEnd(update_finished);
    httpUpdate.onProgress(update_progress);
    httpUpdate.onError(update_error);

    t_httpUpdate_return ret = httpUpdate.update(client, "http://minerback.heltec.cn/halow-http-update-test.bin");
    // Or:
    //t_httpUpdate_return ret = httpUpdate.update(client, "server", 80, "/file.bin");

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
  }
}

