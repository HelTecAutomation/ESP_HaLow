/**********************************************************************
  Filename    : Camera Web Server
  Description : The camera images captured by the ESP32S3 are displayed on the web page through WiFi Halow.
  Auther      : www.freenove.com, heltec_automation
  Modification: 2022/10/31
**********************************************************************/
#include "esp_camera.h"
#include "mmiperf.h"
#include "HaLow.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event_base.h"
#include "esp_event.h"
#include "esp_timer.h"


// ===========================
// Enter your Halow credentials
// ===========================

const char* ssid     = "HT-H7608";
const char* password = "123456789";

ESP_EVENT_DECLARE_BASE(ESP_HTTP_SERVER_EVENT);
//ESP_EVENT_DEFINE_BASE(ESP_HTTP_SERVER_EVENT);
typedef enum {
    HTTP_SERVER_EVENT_ERROR = 0,       /*!< This event occurs when there are any errors during execution */
    HTTP_SERVER_EVENT_START,           /*!< This event occurs when HTTP Server is started */
    HTTP_SERVER_EVENT_ON_CONNECTED,    /*!< Once the HTTP Server has been connected to the client, no data exchange has been performed */
    HTTP_SERVER_EVENT_ON_HEADER,       /*!< Occurs when receiving each header sent from the client */
    HTTP_SERVER_EVENT_HEADERS_SENT,     /*!< After sending all the headers to the client */
    HTTP_SERVER_EVENT_ON_DATA,         /*!< Occurs when receiving data from the client */
    HTTP_SERVER_EVENT_SENT_DATA,       /*!< Occurs when an ESP HTTP server session is finished */
    HTTP_SERVER_EVENT_DISCONNECTED,    /*!< The connection has been disconnected */
    HTTP_SERVER_EVENT_STOP,            /*!< This event occurs when HTTP Server is stopped */
} esp_http_server_event_id_t;

static const char *TAG1 = "camera_test";

void startCameraServer();

void setup() {
  Serial.begin(115200);
//  Serial.setDebugOutput(true);
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
  config.frame_size = FRAMESIZE_SVGA;
  config.fb_location = CAMERA_FB_IN_DRAM;


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
  
  startCameraServer();
  
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(HaLow.localIP());
  Serial.println("' to connect");
}

void loop() {
  // Do nothing. Everything is done in another task by the web server
  delay(10000);
  Serial.printf("[time %u]",millis()/1000);
}
