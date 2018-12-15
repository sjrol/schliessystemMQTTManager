#include "RFID.h"
#include "Network.h"
#include "LED.h"
#include "Keypad.h"

void setup() {
  //init rfid reader
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.begin(9600);

  //Pinmode setzen
  pinMode(D1, OUTPUT);//relais
  digitalWrite(D1, LOW);
  pinMode(D2, OUTPUT);//led r
  digitalWrite(D2, HIGH);
  pinMode(D3, OUTPUT);//led g
  digitalWrite(D3, LOW);

  wifi_connect();
  mqtt_connect();

  led_ready();
}

void loop() {
  network_loop();
  rfid_loop();
  relais_off_loop();
  //keypadLoop();
}
