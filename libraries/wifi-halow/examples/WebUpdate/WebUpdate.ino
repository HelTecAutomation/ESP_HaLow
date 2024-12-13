/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" xxx.xxx.xxx.xxx/update
*/

#include <HaLow.h>
#include <HaLowClient.h>
#include <WebServer.h>
#include <Update.h>

const char* ssid     = "HT-H7608";
const char* password = "123456789";

WebServer server(80);
const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

void setup(void) {
  Serial.begin(115200);
  Serial.println("web update example");
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
  
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.setDebugOutput(true);
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin()) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    } else {
      Serial.printf("Update Failed Unexpectedly (likely broken connection): status=%d\n", upload.status);
    }
  });
  server.begin();
  Serial.print("Ready! Open http://");
  Serial.print(HaLow.localIP());
  Serial.println(" in your browser\n");
}

uint32_t t=0;
void loop(void) {
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks
  if( (millis() - t)>5000)
  {
    t=millis();
    if(HaLow.status() != WL_CONNECTED)
    {
      digitalWrite(LED,0);
    }
    else
    {
      Serial.printf("[time %u s] halow rssi %d\r\n",millis()/1000,HaLow.RSSI());
      digitalWrite(LED,1);
    }
  }
}
