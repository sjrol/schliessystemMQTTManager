#include <ESP8266WiFi.h> //ESP Library
#include <PubSubClient.h>  //MQTT Library
#include "MFRC522.h" //RFID Library
#include "ArduinoOTA.h"


#include "LEDControl.h"
#include "ReceivedMessage.h"
#include "ArduinoCredentials.h"// Wifi & OTA Zugang


#define RST_PIN 16 //REsET PIN RFID Board
#define SS_PIN  2 //Sync PIN RFID Board

MFRC522 mfrc522(SS_PIN, RST_PIN); //RFID Board Setup

String mac = WiFi.macAddress();
int lastread = 0; // debounce RFID
double relaisSet = 0;
double relaisLastSet = 0;

//-------------------------------------
//-------------------------------------

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 is the listener port for the Broker

//-------------------------------------
//-------------------------------------

bool Connect() {
  // Connect to MQTT Server and subscribe to the topic
  if (client.connect((String(mac + "ID").c_str()), mqtt_username, mqtt_password)) {
    client.subscribe((String("/" + mac + "/state").c_str()));
    return true;
  }
  else {
    return false;
  }
}
//------------------------------------
//-------------------------------------
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
    Serial.print(".");
  }

  //DEBUG IP OUTPUT
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // setCallback sets the function to be called when a message is received.
  client.setCallback(ReceivedMessage);
  if (Connect()) {
    Serial.println("Connected Successfully to MQTT Broker!");
  }
  else {
    Serial.println("Connection Failed!");
  }

  client.publish(String("/info").c_str(), String("Hooray, " + mac + " is online now. Hello, my current IP is" + WiFi.localIP()).c_str());
  //=========================================================================================================

  ArduinoOTA.setPort(ota_port);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(String(mac).c_str());

  ArduinoOTA.setPassword(ota_password);

  ArduinoOTA.onStart([]() {
    relaisState = 0 + millis(); //close
    digitalWrite(D1, LOW);
    digitalWrite(D2, LOW);
    digitalWrite(D3, LOW);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    client.publish(String("/info").c_str(), String(mac + "error" + String(error)).c_str());
    if (error == OTA_AUTH_ERROR)  client.publish(String("/info").c_str(), String(mac + "Auth Failed").c_str());
    else if (error == OTA_BEGIN_ERROR) client.publish(String("/info").c_str(), String(mac + "Begin Failed").c_str());
    else if (error == OTA_CONNECT_ERROR) client.publish(String("/info").c_str(), String(mac + "Connect Failed").c_str());
    else if (error == OTA_RECEIVE_ERROR) client.publish(String("/info").c_str(), String(mac + "Receive Failed").c_str());
    else if (error == OTA_END_ERROR) client.publish(String("/info").c_str(), String(mac + "End Failed").c_str());
  });
  ArduinoOTA.begin();
}

//-------------------------------------
//-------------------------------------
void loop() {

  //wifi reconnect
  if (!client.connected()) Connect();

  //MQTT Abfrage rx tx
  if(!client.loop()) Connect();

  if (updateState - millis() >= 1) ArduinoOTA.handle();


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


    long code = 0;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      code = ((code + mfrc522.uid.uidByte[i]) * 10);
    }
    char longcode [15];
    sprintf(longcode,"%#015d",code);
    client.publish(String("/" + mac).c_str(), longcode);
    lastread = millis();
  } else if (messagerecieved == 0) {
    LEDControl(3);
  } else {
    //blank
  }
}
