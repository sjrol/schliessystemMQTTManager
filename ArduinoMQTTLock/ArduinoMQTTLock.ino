const int FW_VERSION = 1013;  //Firmware Muss identisch zu arduino.version sein 
#include "Arduino.h"
#include <ESP8266WiFi.h> //ESP Library
#include <WebSocketClient.h> //Websockets Library
#include "MFRC522.h" //RFID Library

#include <ESP8266HTTPClient.h> //Library fuer http connect
#include <ESP8266httpUpdate.h> //Library fuer http Update

#include "LEDControl.h" //Void fuer Led Setting
#include "ReceivedMessage.h" //Void fuer MQTT input
#include "ArduinoCredentials.h"// Wifi & OTA Zugang

#define RST_PIN 16 //RSsET PIN RFID Board
#define SS_PIN  2 //Sync PIN RFID Board

MFRC522 mfrc522(SS_PIN, RST_PIN); //RFID Board Setup

String mac = WiFi.macAddress(); // Setzte MAC adresse als mac 
int lastread = 0; // debounce RFID
double relaisSet = 0; //relais zeit (diese Var. beschreibt, wie lange das relais geoeffnet bleibt in ms)
double relaisLastSet = 0; //relais referenzm (diese Var. ist die Referenz, auf die sich relaisSet bezieht)

//-------------------------------------
//-------------------------------------

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;  // ESP8266 soll als WLAN client fungieren(keinen AP oeffnen)

char server[] = "echo.websocket.org";
WebSocketClient client;



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

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);
  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  client.connect(websocket_server);
  client.setDataArrivedDelegate(ReceivedMessage);
 
  }

// Include Updater nach der gesammmten init
#include "checkForUpdates.h"

//-------------------------------------
//-------------------------------------
void loop() {

  //wifi   if (!client.connected()) Connect(); //wenn WLAN nicht verbunden, versuche reconnect

  client.monitor();

  
// Update checker
  if (updateState - millis() >= 1) { //loest aus, wenn ein u per MQTT eingegangen ist
    checkForUpdates();
  }


  //relais state setzen
  if (relaisState - millis() >= 1) {
    digitalWrite(D1, HIGH);
    LEDControl(2);
  } else {
    digitalWrite(D1, LOW);
    LEDControl(0);
    messagerecieved = 0;
  };

  // Look for new cards
  if (lastread + 2000 < millis()) {
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      delay(200);
      return;
    }
    Serial.println("\nEnd"); 
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      delay(50);
      return;
    }


    String code = ""; //leert die Variable 
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      code = String(mfrc522.uid.uidByte[i], HEX) + code; //baut die UID in HEX 
      if(code.length()<(i+1)*2) {
        code = String("0") + code ;
      }
    }
    client.send(String(code)); //sendet die UID via MQTT
    lastread = millis();
  } else if (messagerecieved == 0) {
    LEDControl(3); 
  } else {
    //blank
  }
}
