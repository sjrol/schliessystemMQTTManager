#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Config.h"
#include "Message.h"

WiFiClient wifiClient;
PubSubClient client(mqtt_server, mqtt_port, wifiClient);

String mac = WiFi.macAddress();

bool wifi_connect() {
  Serial.println("Connecting WiFi ...");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi already connected.");
    return false;
  }
  
  // Connect to the WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, wifi_password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Connected. Print IP
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("WiFi connected.");

  return true;
}

bool mqtt_connect() {
  Serial.println("Connecting MQTT ...");
  if (client.connected()) {
    Serial.println("MQTT already connected.");
    return false;
  }
  
  // Connect to MQTT Server and subscribe to the topic
  client.setCallback(onReceive);
  if (client.connect((String(mac).c_str()), mqtt_username, mqtt_password)) { // Connect with MAC as client ID
    client.subscribe((String("/" + mac + "/state").c_str())); // Subscribe to topic /MAC/state
    client.publish(String("/info").c_str(), String("Hooray, " + mac + " is online now. Hello, my current IP is " + WiFi.localIP().toString()).c_str());
    Serial.println("MQTT connected.");
    return true;
  }
  
  return false;
}

bool mqtt_publish(const char* payload) {
  client.publish(String("/" + mac).c_str(), payload);
}

void network_loop() {
  if (WiFi.status() != WL_CONNECTED)
    wifi_connect();
  if (!client.loop())
    mqtt_connect();
}

#endif
