#!/bin/python

import paho.mqtt.client as mqtt
import requests,json,time
import credentials

MQTT_TOPIC_BASE = "A0:20:A6:14:D7:7A"

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

def on_message(client, userdata, msg):
    print("recived message \"" + str(msg.payload.decode('utf-8')) + "\" in " + str(msg.topic))
    if str(msg.payload.decode('utf-8')) =="1301970":
            client.publish(str(msg.topic)+"/state", str('s'))
    else :
            client.publish(str(msg.topic)+"/state", str("l"))

print(credentials.mqttBrokerURL)
client = mqtt.Client()
client.username_pw_set(credentials.mqtt_username, credentials.mqtt_password)
client.on_connect = on_connect
client.on_message = on_message
client.connect(host=credentials.mqttBrokerURL)
client.subscribe(MQTT_TOPIC_BASE,2)
client.loop_forever()

