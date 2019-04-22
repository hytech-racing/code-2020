#!/usr/bin/env python3

###
# HyTech Racing - MQTT/LTE/Firebase Telemetry broker
# Written Spring 2019. Contact: Brent Bessemer <bbessemer3@gatech.edu>
#
# This code recieves MQTT messages containing CAN messages from the MQTT
# broker (which will probably be on the same machine) and publishes them
# to Firebase.
###

import time
import paho.mqtt.client as mqtt
import firebase_admin
from firebase_admin import credentials
from firebase_admin import db
import hytech_can as can

# Fetch the service account key JSON file contents
cred = credentials.Certificate('firebase-private-key.json')

# Initialize the app with a service account, granting admin privileges
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://hytech-telemetry.firebaseio.com'
})

cv_ref = db.reference('/canBus/currentValues')
session_ref = db.reference('/canBus/sessions').push({
    'start': int(time.time()),
    'messages': {}
})

def mqtt_connect(client, userdata, flags, rc):
    print('Connecting')
    client.subscribe("hytech_car/telemetry")
    client.publish("hytech_car/telemetry", "Firebase client connected")

def mqtt_message(client, userdata, msg):
    data = can.unpack(msg.payload)
    can_msg = can.decode(data)
    print(can_msg)
    session_ref.child('messages').push(can_msg)

    for key in can_msg['data']:
        cv_ref.child(key).set(can_msg['data'][key])

client = mqtt.Client()
client.connect("hytech-telemetry.ryangallaway.me", 1883, 60)
client.on_connect = mqtt_connect
client.on_message = mqtt_message
client.loop_start()

while True:
    try:
        continue
    except (KeyboardInterrupt, SystemExit):
        client.loop_stop()
        client.disconnect() # TODO unsure if this should be called
        session_ref.child('end').set(int(time.time()))
        break
