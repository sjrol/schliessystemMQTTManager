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
    top = str(msg.topic)
    MAC=top.replace(":","").upper().replace("/","")
    print("recived message \"" + token + "\" in " + top)
    cursor.execute("SELECT `relay`.`mac` FROM `token` INNER JOIN `user` ON(`token`.`user` = `user`.`id`) INNER JOIN `user_relay` ON(`user`.`id` = `user_relay`.`user`) INNER JOIN `relay` ON(`user_relay`.`relay` = `relay`.`id`) INNER JOIN `reader_relay` ON(`relay`.`id` = `reader_relay`.`relay`) INNER JOIN `reader` ON(`reader_relay`.`reader` = `reader`.`id`) WHERE `token`.`id` = '%s' AND `reader`.`mac` = '%s'" % (token,MAC))
    sucess = False
    for mac in cursor:
        if "%s"%mac == MAC:
            client.publish(top+"/state", str('s'))
            sucess = True
            print("Access granted for %s"%token)
        else:
            client.publish(top+"/state", str('e'))
            print("Acess denied for %s"%token)
    if not(sucess):
        client.publish(top+"/state", str('e'))
        print("Acess denied for %s"%token)

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

