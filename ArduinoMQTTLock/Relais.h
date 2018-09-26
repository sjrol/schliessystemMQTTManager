#ifndef __RELAIS_H__
#define __RELAIS_H__

#include "LED.h"

unsigned long relais_closeAfter = 0;
bool relais_wasOff = true;

void relais_on(unsigned long msec) {
  Serial.println("Relais on for " + String(msec) + " milliseconds.");
  relais_closeAfter = msec + millis();
  relais_wasOff = false;
  led_set('g');
  digitalWrite(D1, HIGH);
}

void relais_on(byte* payload) {
  relais_on(long((char*)payload));
}

void relais_off() {
  if(relais_wasOff)
    return;
  relais_wasOff = true;
  Serial.println("Relais off");
  digitalWrite(D1, LOW);
  led_set('0');
}

void relais_off_loop() {
  if (relais_closeAfter < millis()) {
    relais_off();
  }
}

#endif
