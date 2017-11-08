#include <ESP8266WiFi.h> //ESP Library
#include <PubSubClient.h>  //MQTT Library
#include "MFRC522.h" //RFID Library



#define RST_PIN 16 //REsET PIN RFID Board
#define SS_PIN  2 //Sync PIN RFID Board


MFRC522 mfrc522(SS_PIN, RST_PIN); //RFID Board Setup


const char* ssid = "***";  //Wlan ssid setzen
const char* wifi_password = "***"; // Wlan Passwort setzen

const char* mqtt_server = "***.***.***.***";// MQTT Broker Server IP setzen
const char* mqtt_txtopic = "/door/buero/id"; // MQTT Publish Topic setzen
const char* mqtt_rxtopic = "/door/buero/id/state"; // MQTT Subscribe Topic setzen
const char* mqtt_username = "user"; //MQTT PW
const char* mqtt_password = "admin"; //MQTT Username
const char* clientID = "t1"; //MQTT Client id

double relaisState = 0; // open / close
int lastread = 0; // debounce RFID
//-------------------------------------
//-------------------------------------


// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 is the listener port for the Broker
//-------------------------------------
//-------------------------------------
//void uebernimmt rx vom mqtt server 
//switch setzt zeiten/error
void ReceivedMessage(char* topic, byte* payload, unsigned int length) {//Setzt neue Nachrichten des rx Topics in Relaiszeiten um
    switch((char)payload[0]){
      case 's':
        relaisState = 3000+millis();//Kurz 3 sec
        break;
      case 'm':
        relaisState = 6000+millis();//Mittel 6 sec
        break;
      case 'l':
        relaisState = 12000+millis();//Lang 12 sec
        break;
      case 'o':
        relaisState = 86400000 + millis();//Dauer offen (1d)
        break;
      case 'c':
        relaisState = 0+millis();//close
        break;
      case 'e':
        LEDControl(1);
        delay(2000);
      default:
        break;                
    } 
}


//void setzt status led r g rg=y 
void LEDControl(int color){
  switch(color){
      case '0':
          digitalWrite(D2, LOW);
          digitalWrite(D3, LOW);
        break;
      case '1':
          digitalWrite(D2, HIGH);
          digitalWrite(D3, LOW);
        break;
      case '2':
          digitalWrite(D2, LOW);
          digitalWrite(D3, HIGH);
         break;
      case '3':
          digitalWrite(D2, HIGH);
          digitalWrite(D3, HIGH);
        break;
      default:
          digitalWrite(D2, LOW);
          digitalWrite(D3, LOW);
        break;                
    }
}
//-------------------------------------
//-------------------------------------
bool Connect() {
  // Connect to MQTT Server and subscribe to the topic
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
      client.subscribe(mqtt_rxtopic);
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
  digitalWrite(D2, LOW);
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

}
//-------------------------------------
//-------------------------------------
void loop() {

  //wifi reconnect
  if (!client.connected()) {
    Connect();
  }

  
  //MQTT Abfrage rx tx
  client.loop();


  //relais state setzen
  if(relaisState - millis() >= 1) {
    digitalWrite(D1, HIGH);
    LEDControl(2);
   }else{
    digitalWrite(D1, LOW); 
    LEDControl(0); 
   };

   // Look for new cards
   if(lastread+2000 < millis()){
              if ( ! mfrc522.PICC_IsNewCardPresent()) {
                delay(500);
                return;
              }
              // Select one of the cards
              if ( ! mfrc522.PICC_ReadCardSerial()) {
                delay(50);
                return;
              }
            
            
              long code=0;
              for (byte i = 0; i < mfrc522.uid.size; i++){
              code=((code+mfrc522.uid.uidByte[i])*10);
              }
              client.publish(mqtt_txtopic, String(code).c_str());
              lastread = millis();
   }else{
    LEDControl(3);
   }
}   
