#ifndef __RFID_H__
#define __RFID_H__

#include "MFRC522.h" //RFID Library
#include "Network.h"

#define RST_PIN 16 //RSsET PIN RFID Board
#define SS_PIN  2 //Sync PIN RFID Board

MFRC522 mfrc522(SS_PIN, RST_PIN); //RFID Board Setup

unsigned long rfid_nextReadAfter = 0;

void rfid_loop() {
  if (!(rfid_nextReadAfter < millis()))
    return;
  if (!mfrc522.PICC_IsNewCardPresent()) {
    rfid_nextReadAfter = millis() + 200;
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    rfid_nextReadAfter = millis() + 50;
    return;
  }
    
  String code = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    code = String(mfrc522.uid.uidByte[i], HEX) + code;
    if(code.length()<(i+1)*2) {
      code = String("0") + code;
    }
  }

  Serial.print("Token read: ");
  Serial.println(code);
  led_set('y');
  mqtt_publish(code.c_str());
  rfid_nextReadAfter = millis() + 2000;
}

#endif
