#ifndef __OTA_H__
#define __OTA_H__

#include <ESP8266httpUpdate.h> //Library fuer http Update

#include "Config.h"

void ota() {
  Serial.println("Update starting ...");
  t_httpUpdate_return ret = ESPhttpUpdate.update(updateUrl, String(VERSION).c_str());
  switch(ret) {
    case HTTP_UPDATE_FAILED:
       Serial.println("Update failed.");
       Serial.println(ESPhttpUpdate.getLastErrorString());
       break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("Update not available.");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("Update ok."); // may not called we reboot the ESP
      break;
  }
}

#endif
