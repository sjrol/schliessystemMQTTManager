#!/bin/python

import paho.mqtt.client as mqtt
import requests,json,time

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

def on_message(client, userdata, msg):
    print("recived message \"" + str(msg.payload.decode('utf-8')) + "\" in " + str(msg.topic))
    if str(msg.payload.decode('utf-8')) =="1301970":
            client.publish(str(msg.topic)+"/state", str('s'))
    else :
            client.publish(str(msg.topic)+"/state", str("l"))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(host="s3.sjr-ol.de")
client.subscribe("/door/buero/id",2)
client.loop_forever()


