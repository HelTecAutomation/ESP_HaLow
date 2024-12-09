#include "HaLow.h"
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  // Set HaLow region. 
  HaLow.init("US");
  
  Serial.println("Setup done");
}

void loop() {
  Serial.println("Scan start");

  // HaLow.scanNetworks will return the number of networks found.
  int n = HaLow.scanNetworks();
  Serial.println("Scan done");

  if (n == 0) {
      Serial.println("no networks found");
  } else {
      Serial.print(n);
      Serial.println(" networks found");
      Serial.printf("Nr | %-32.32s | RSSI | %-17.17s | Encryption\r\n","SSID","BSSID");
      for (int i = 0; i < n; i++) {
          // Print SSID and RSSI for each network found
          Serial.printf("%2d",i + 1);
          Serial.print(" | ");
          Serial.printf("%-32.32s", HaLow.SSID(i).c_str());
          Serial.print(" | ");
          Serial.printf("%4ld", HaLow.RSSI(i));
          Serial.print(" | ");
          Serial.print(HaLow.BSSIDstr(i));
          Serial.print(" | ");

          //the follow can be simply replace with "Serial.print(HaLow.encryptionTypeStr(i));"
          switch (HaLow.encryptionType(i))
          {
          case MMWLAN_OPEN:
              Serial.print("OPEN");
              break;
          case MMWLAN_OWE:
              Serial.print("OWE");
              break;
          case MMWLAN_SAE:
              Serial.print("SAE");
              break;
          case MMWLAN_OTHER:
          default:
              Serial.print("UNKNOWN");
          }
          Serial.println();
          delay(10);
      }
  }

  Serial.println("");
  
  // Delete the scan result to free memory for code below.
  HaLow.scanDelete();
  
  // Wait a bit before scanning again.
  delay(5000);
}
