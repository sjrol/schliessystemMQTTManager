#!/usr/bin/python3

import paho.mqtt.client as mqtt
import requests,json,time
import credentials
import mysql.connector as mariadb
import re

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

def on_message(client, userdata, msg):
    topic = str(msg.topic)
    token = str(msg.payload.decode('utf-8'))
    try:
        reader = reader_from_topic.match(topic).group(1).upper()
    except AttributeError as err:
        print(err)
        return
    print("Got Token „%s“ from Reader „%s“ on Topic „%s“" % (token, reader, topic))
    cursor = db.cursor()
    cursor.execute("SELECT `relay`.`id`, `relay`.`mac`, `user`.`id`, `user`.`name`, `reader`.`id`, `reader`.`name`, `relay`.`name` FROM `token` INNER JOIN `user` ON(`token`.`user` = `user`.`id`) INNER JOIN `user_relay` ON(`user`.`id` = `user_relay`.`user`) INNER JOIN `relay` ON(`user_relay`.`relay` = `relay`.`id`) INNER JOIN `reader_relay` ON(`relay`.`id` = `reader_relay`.`relay`) INNER JOIN `reader` ON(`reader_relay`.`reader` = `reader`.`id`) WHERE `token`.`id` = '%s' AND `reader`.`mac` = '%s'" % (token, reader))
    success = False
    logstr = ""
    _relayId = 0 # Corona
    _userId = 0 # Corona
    for (relayId, relayMac, userId, userName, readerId, readerName, relayName) in cursor:
        client.publish("/door/%s" % (relayMac), str('s'))
        logstr += ",('%s','%s','%s','%s')" % (userId, readerId, relayId, 's')
        success = True
        print("Access granted for user %s on door %s with reader %s" % (userName, relayName, readerName))
        _relayId = relayId # Corona
        _userId = userId # Corona
    if (success):
        client.publish("/reader/%s" % (reader), str('s'))
        log = db.cursor()
        log.execute("INSERT INTO `log` (`user`,`reader`,`relay`,`result`) VALUES %s" % (logstr[1:]))
        log.close()
        if _relayId == 1: # Corona
            corona_checkout = db.cursor()
            corona_checkout.execute("SELECT COUNT(*) FROM `log` WHERE `user` = %s AND `relay` = 1 AND `timestamp` > CURRENT_TIMESTAMP()-10" % (_userId))
            corona_row = corona_checkout.fetchone()
            if corona_row is not None:
                corona_count = corona_row[0]
                if corona_count > 1:
                    client.publish("/reader/%s" % (reader), str('p'))
                    print("Corona-Checkout performed for user %s on door %s with reader %s" % (userName, relayName, readerName))
            corona_checkout.close()
    else:
        client.publish("/reader/%s" % (reader), str('e'))
        log = db.cursor()
        log.execute("INSERT INTO `log` (`user`,`reader`,`result`) VALUES ((SELECT `user` FROM `token` WHERE `id` = '%s'), (SELECT `id` FROM `reader` WHERE `mac` = '%s'), 'e')" % (token, reader))
        log.close()
        print("Access denied for token %s" % (token))
    cursor.close()
    db.commit()

reader_from_topic = re.compile(r'/reader/(([A-Fa-f0-9]{2}[:]){5}[A-Fa-f0-9]{2})/token')
db = mariadb.connect(user = credentials.mariadbUser, password = credentials.mariadbPw, host =credentials.mariadbServer, database=credentials.mariadbUser)
print("MQTT-Broker: " + credentials.mqttBrokerURL)
client = mqtt.Client()
client.username_pw_set(credentials.mqtt_username, credentials.mqtt_password)
client.on_connect = on_connect
client.on_message = on_message
client.connect(host=credentials.mqttBrokerURL)
client.subscribe('/reader/+/token', 2)
client.loop_forever()
