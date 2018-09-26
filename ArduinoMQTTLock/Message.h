#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "LED.h"
#include "OTA.h"
#include "Relais.h"

void onReceive(const char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  Serial.println("Message received: " + String((char*)payload));
  switch ((char)payload[0]) {
    case 's': // Open 3s
      relais_on(3000);
      break;
    case 'm': // Open 6s
      relais_on(6000);
      break;
    case 'l': // Open 12s
      relais_on(12000);
      break;
    case 'o': // Open 24h
      relais_on(86400000);
      break;
    case 't':
      relais_on(payload[1]);
      break;
    case 'c': // Close
      relais_off();
      break;
    case 'e': // Error
      led_error();
      break;
    case 'u': // Update
      ota();
      break;
    case 'r': // Reboot
      ESP.restart();
      break;
    case 'p': // LED party mode
      led_party();
      break;
    default: // Unknown command
      led_blink('y', 4);
      break;
  }
}

#endif
