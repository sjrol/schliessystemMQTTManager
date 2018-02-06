#ifndef __CHECKFORUPDATES_H__
#define __CHECKFORUPDATES_H__

void checkForUpdates() {
  String mac = WiFi.macAddress();
  String fwURL = String( fwUrlBase );
  fwURL.concat( mac );
  String fwVersionURL = fwURL;
  fwVersionURL.concat( ".version" );

  client.publish(String("/info").c_str(), "Checking for firmware updates." );
  client.publish(String("/info").c_str(), "Firmware version URL: " );
  client.publish(String("/info").c_str(), String(fwVersionURL).c_str());

  HTTPClient httpClient;
  httpClient.begin( fwVersionURL );
  int httpCode = httpClient.GET();
  if( httpCode == 200 ) {
    String newFWVersion = httpClient.getString();

    int newVersion = newFWVersion.toInt();

    if( newVersion > FW_VERSION ) {
      client.publish(String("/info").c_str(), "Preparing to update" );

      String fwImageURL = fwURL;
      fwImageURL.concat( ".bin" );
      t_httpUpdate_return ret = ESPhttpUpdate.update( fwImageURL );

      switch(ret) {
        case HTTP_UPDATE_FAILED:
          client.publish(String("/info").c_str(), String("HTTP_UPDATE_FAILD Error " + String(ESPhttpUpdate.getLastError()) + String(ESPhttpUpdate.getLastErrorString()) ).c_str() );
          break;

        case HTTP_UPDATE_NO_UPDATES:
          client.publish(String("/info").c_str(), "HTTP_UPDATE_NO_UPDATES" );
          break;
      }
    }
    else {
      client.publish(String("/info").c_str(), "Already on latest version" );
    }
  }
  else {
    client.publish(String("/info").c_str(), "Firmware version check failed, got HTTP response code " );
    client.publish(String("/info").c_str(), String(httpCode).c_str() );
  }
  httpClient.end();
}

#endif
