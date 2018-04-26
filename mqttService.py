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
    cursor = db.cursor()
    cursor.execute("SELECT `relay`.`id`, `relay`.`mac`, `user`.`id`, `user`.`name`, `reader`.`id`, `reader`.`name`, `relay`.`name` FROM `token` INNER JOIN `user` ON(`token`.`user` = `user`.`id`) INNER JOIN `user_relay` ON(`user`.`id` = `user_relay`.`user`) INNER JOIN `relay` ON(`user_relay`.`relay` = `relay`.`id`) INNER JOIN `reader_relay` ON(`relay`.`id` = `reader_relay`.`relay`) INNER JOIN `reader` ON(`reader_relay`.`reader` = `reader`.`id`) WHERE `token`.`id` = '%s' AND `reader`.`mac` = '%s'" % (token, reader))
    success = False
    logstr = ""
    for (relayId, relayMac, userId, userName, readerId, readerName, relayName) in cursor:
        client.publish("/%s/state" % (relayMac), str('s'))
        logstr += ",('%s','%s','%s','%s')" % (userId, readerId, relayId, 's')
        success = True
        print("Access granted for user %s on door %s with reader %s" % (userName, relayName, readerName))
    if (success):
        client.publish(topic+"/state", str('s'))
        log = db.cursor()
        log.execute("INSERT INTO `log` (`user`,`reader`,`relay`,`result`) VALUES %s" % (logstr[1:]))
        log.close()
    else:
        client.publish(topic+"/state", str('e'))
        print("Access denied for token %s" % (token))
    cursor.close()
    db.commit()

db = mariadb.connect(user = credentials.mariadbUser, password = credentials.mariadbPw, host =credentials.mariadbServer, database=credentials.mariadbUser)
print("MQTT-Broker: " + credentials.mqttBrokerURL)
client = mqtt.Client()
client.username_pw_set(credentials.mqtt_username, credentials.mqtt_password)
client.on_connect = on_connect
client.on_message = on_message
client.connect(host=credentials.mqttBrokerURL)
client.subscribe(MQTT_TOPIC_BASE,2)
client.loop_forever()

