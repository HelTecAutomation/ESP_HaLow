/**
 * BasicHTTPClient.ino
 *
 *  Created on: 20.04.2024
 *
 */

#include <Arduino.h>
#include <HalowHTTPClient.h>
#include <HaLow.h>

#define HALOW_REGION     "US"
#define HALOW_AP_SSID    "HaLow-AP"
#define HALOW_PASSWORD   "heltec.org"

const char *rootCACertificate = R"string_literal(
-----BEGIN CERTIFICATE-----
MIIDajCCAxGgAwIBAgIQHaevXdgqHrEODAAxEpFh9DAKBggqhkjOPQQDAjA7MQsw
CQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZpY2VzMQwwCgYD
VQQDEwNXRTEwHhcNMjYwOTA2MDMzNzQ3WhcNMjYxMjA1MDQzNzMyWjARMQ8wDQYD
VQQDEwZ3My5vcmcwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAAQoBIOj3pn7taW9
raiRVf0+843lGGE8xSd9qCrk+8xR46HL0KdXed++rlLy8U5y5AipS32OChsltsXe
8xO0Wdu1o4ICHzCCAhswDgYDVR0PAQH/BAQDAgeAMBMGA1UdJQQMMAoGCCsGAQUF
BwMBMAwGA1UdEwEB/wQCMAAwHQYDVR0OBBYEFNNbj+OfhKvWBRpOyQ8tnLcPbWtH
MB8GA1UdIwQYMBaAFJB3kjVnxP+ozKnme9mAeXvMk/k4MDUGCCsGAQUFBwEBBCkw
JzAlBggrBgEFBQcwAoYZaHR0cDovL2kucGtpLmdvb2cvd2UxLmNydDAbBgNVHREE
FDASggZ3My5vcmeCCCoudzMub3JnMBMGA1UdIAQMMAowCAYGZ4EMAQIBMDYGA1Ud
HwQvMC0wK6ApoCeGJWh0dHA6Ly9jLnBraS5nb29nL3dlMS9MdDJ6YkwxNnpXVS5j
cmwwggEDBgorBgEEAdZ5AgQCBIH0BIHxAO8AdgDIo8R/x7OtuTVrAT9qehJt4zpO
Q6XGRvmXrTl1mR3PmgAAAaB1Ah07AAAEAwBHMEUCID/K1lpAIgFULR75+u2V6+bk
CickatC0HaKXNDzflgbRAiEA8cVn5Xry31ErvreXrrSZqjTLOlgQkV2bL5xZeAVN
JpcAdQDYCVU7lE96/8gWGW+UT4WrsPj8XodVJg8V0S5yu0VLFAAAAaB1Ah16AAAE
AwBGMEQCIGPXa3whTkBgr6gFVMI7b7QEHtzOmwo8gpvNU/HaI5QkAiBdl6THmiDw
eU/snzGhGqMreO7VywSbCyZCdDfLbpMFXDAKBggqhkjOPQQDAgNHADBEAiACU1i0
Jsd6iXs6qRkVRIfDQZOLvHPvdFOJ9eahDMyDDQIgY11HLrqhnJTOrdDfMVZ9LbJf
DUUq2uYow6wKxF7TFmI=
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIICnzCCAiWgAwIBAgIQf/MZd5csIkp2FV0TttaF4zAKBggqhkjOPQQDAzBHMQsw
CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU
MBIGA1UEAxMLR1RTIFJvb3QgUjQwHhcNMjMxMjEzMDkwMDAwWhcNMjkwMjIwMTQw
MDAwWjA7MQswCQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZp
Y2VzMQwwCgYDVQQDEwNXRTEwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARvzTr+
Z1dHTCEDhUDCR127WEcPQMFcF4XGGTfn1XzthkubgdnXGhOlCgP4mMTG6J7/EFmP
LCaY9eYmJbsPAvpWo4H+MIH7MA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggr
BgEFBQcDAQYIKwYBBQUHAwIwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQU
kHeSNWfE/6jMqeZ72YB5e8yT+TgwHwYDVR0jBBgwFoAUgEzW63T/STaj1dj8tT7F
avCUHYwwNAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzAChhhodHRwOi8vaS5wa2ku
Z29vZy9yNC5jcnQwKwYDVR0fBCQwIjAgoB6gHIYaaHR0cDovL2MucGtpLmdvb2cv
ci9yNC5jcmwwEwYDVR0gBAwwCjAIBgZngQwBAgEwCgYIKoZIzj0EAwMDaAAwZQIx
AOcCq1HW90OVznX+0RGU1cxAQXomvtgM8zItPZCuFQ8jSBJSjz5keROv9aYsAm5V
sQIwJonMaAFi54mrfhfoFNZEfuNMSQ6/bIBiNLiyoX46FohQvKeIoJ99cx7sUkFN
7uJW
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIICCTCCAY6gAwIBAgINAgPlwGjvYxqccpBQUjAKBggqhkjOPQQDAzBHMQswCQYD
VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEUMBIG
A1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAwMDAw
WjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2Vz
IExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQAIgNi
AATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzuhXyi
QHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/lxKvR
HYqjQjBAMA4GA1UdDwEB/wQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQW
BBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNpADBmAjEA6ED/g94D
9J+uHXqnLrmvT/aDHQ4thQEd0dlq7A/Cr8deVl5c1RxYIigL9zC2L7F8AjEA8GE8
p/SgguMh1YQdc4acLa/KNJvxn7kjNuK8YAOdgLOaVsjh4rsUecrNIdSUtUlD
-----END CERTIFICATE-----
)string_literal";

void setup() {
  Serial.begin(115200);
  
#ifdef HT_RC3268
  //enable WiFiHalow LDO
  pinMode(HALOW_LDO_CTRL,OUTPUT);
  digitalWrite(HALOW_LDO_CTRL,HALOW_LDO_ENABLE);
#endif

  HaLow.init(HALOW_REGION);
  HaLow.begin(HALOW_AP_SSID, HALOW_PASSWORD);
  
  // wait for HaLow connection
  Serial.print("Waiting for HaLow to connect...");
  while (HaLow.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" connected");
}

void loop() {
  HalowClientSecure *client = new HalowClientSecure;
  if(client) {
    client -> setCACert(rootCACertificate);

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before HalowClientSecure *client is 
      HalowHTTPClient https;
  
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, "https://jigsaw.w3.org/HTTP/connection.html")) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println(payload);
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      // End extra scoping block
    }
  
    delete client;
  } else {
    Serial.println("Unable to create client");
  }

  Serial.println();
  Serial.println("Waiting 10s before the next round...");
  delay(10000);
}
