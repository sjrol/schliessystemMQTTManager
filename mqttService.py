#!/usr/bin/python3

import paho.mqtt.client as mqtt
import requests,json,time
import credentials
import mysql.connector as mariadb

MQTT_TOPIC_BASE = "/+"
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

def on_message(client, userdata, msg):
    token = str(msg.payload.decode('utf-8'))
    topic = str(msg.topic)
    reader = topic[1:].upper()
    cursor.execute("SELECT `relay`.`mac`, `user`.`name`, `reader`.`name`, `relay`.`name` FROM `token` INNER JOIN `user` ON(`token`.`user` = `user`.`id`) INNER JOIN `user_relay` ON(`user`.`id` = `user_relay`.`user`) INNER JOIN `relay` ON(`user_relay`.`relay` = `relay`.`id`) INNER JOIN `reader_relay` ON(`relay`.`id` = `reader_relay`.`relay`) INNER JOIN `reader` ON(`reader_relay`.`reader` = `reader`.`id`) WHERE `token`.`id` = '%s' AND `reader`.`mac` = '%s'" % (token, reader))
    success = False
    for (relayMac, userName, readerName, relayName) in cursor:
        client.publish("/%s/state" % (relayMac), str('s'))
        success = True
        print("Access granted for user %s on door %s with reader %s" % (userName, relayName, readerName))
    if (success):
        client.publish(topic+"/state", str('s'))
    else:
        client.publish(topic+"/state", str('e'))
        print("Access denied for token %s" % (token))

cnx = mariadb.connect(user = credentials.mariadbUser, password = credentials.mariadbPw, host =credentials.mariadbServer, database=credentials.mariadbUser) 
cursor = cnx.cursor()
print("MQTT-Broker: " + credentials.mqttBrokerURL)
client = mqtt.Client()
client.username_pw_set(credentials.mqtt_username, credentials.mqtt_password)
client.on_connect = on_connect
client.on_message = on_message
client.connect(host=credentials.mqttBrokerURL)
client.subscribe(MQTT_TOPIC_BASE,2)
client.loop_forever()

