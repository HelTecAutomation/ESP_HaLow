/**********************************************************************
  Filename    : Video Web Server
  Description : The camera images captured by the ESP32S3 are displayed on the web page.
  Auther      : www.freenove.com
  Modification: 2022/11/01
**********************************************************************/
#include "esp_camera.h"
#include <HaLow.h>
#include "sd_read_write.h"

const char* ssid     = "MorseMicro2";
const char* password = "123456789";
void cameraInit(void);
void startCameraServer();

#define LED 20

void setup() {
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,0);
  delay(100);
  Serial.setDebugOutput(true);
  Serial.println();

  cameraInit();
  sdmmcInit();
  removeDir(SD, "/video");
  createDir(SD, "/video");
  
  Serial.println("Start WiFi HaLow");
  HaLow.init("US");
  HaLow.begin(ssid, password);
  Serial.print("MAC:");
  Serial.print(HaLow.macAddress());
  Serial.println();
  
  while (HaLow.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(); 

  Serial.println("");
  Serial.println("HaLow connected");
  digitalWrite(LED,1);

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(HaLow.localIP());
  Serial.println("' to connect");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(5000);

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

void cameraInit(void){
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  // for larger pre-allocated frame buffer.
  if(psramFound()){
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    // Limit the frame size when PSRAM is not available
    config.frame_size = FRAMESIZE_SVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  s->set_vflip(s, 1); // flip it back
  s->set_brightness(s, 1); // up the brightness just a bit
  s->set_saturation(s, 0); // lower the saturation
}
