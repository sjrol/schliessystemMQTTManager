#ifndef __CONFIG_H__
#define __CONFIG_H__

#define VERSION "v0.20"

const char* ssid = "WLAN-SJR";  //Wlan ssid setzen
const char* wifi_password = "SJR1994hdj"; // Wlan Passwort setzen

const char* mqtt_username = "user"; //MQTT PW
const char* mqtt_password = "admin"; //MQTT Username
const char* mqtt_server = "136.243.24.244";// MQTT Broker Server IP setzen
const unsigned int mqtt_port = 1883;

const char* updateUrl = "http://doors.sjr-ol.de/";

#endif
