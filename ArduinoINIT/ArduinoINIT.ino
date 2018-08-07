const int FW_VERSION = 1000;  //Firmware Muss identisch zu arduino.version sein 

#include <ESP8266WiFi.h> //ESP Library

#include <ESP8266HTTPClient.h> //Library fuer http connect
#include <ESP8266httpUpdate.h> //Library fuer http Update
#include "ArduinoCredentials.h"// Wifi & OTA Zugang

WiFiClient wifiClient;  // ESP8266 soll als WLAN client fungieren(keinen AP oeffnen)

void setup() {



  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);
  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}
void loop() {
  
  String fwURL = String( fwUrlBase );
  fwURL.concat( "arduino" );
  String fwVersionURL = fwURL;
  fwVersionURL.concat( ".version" );



  HTTPClient httpClient;
  httpClient.begin( fwVersionURL );
  int httpCode = httpClient.GET();
  if( httpCode == 200 ) {
    String newFWVersion = httpClient.getString();

    int newVersion = newFWVersion.toInt();

    if( newVersion > FW_VERSION ) {
      String fwImageURL = fwURL;
      fwImageURL.concat( ".bin" );
      t_httpUpdate_return ret = ESPhttpUpdate.update( fwImageURL );

      switch(ret) {
        case HTTP_UPDATE_FAILED:
          break;

        case HTTP_UPDATE_NO_UPDATES:
          break;
      }
    }
  }
}
