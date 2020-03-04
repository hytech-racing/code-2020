import sys
import datetime
import time
import binascii
from cobs import cobs
import codecs
# import msvcrt
import threading
import struct
import paho.mqtt.client as mqtt
from influxdb import InfluxDBClient
from decoder import decode
import requests

MQTT_SERVER = 'localhost'
# MQTT_SERVER = 'ec2-3-134-2-166.us-east-2.compute.amazonaws.com'
# MQTT_SERVER = 'hytech-telemetry.ryangallaway.me'     # MQTT broker hostname

MQTT_PORT = 1883                    # MQTT broker port (non-SSL)
MQTT_TOPIC = 'hytech_car/telemetry'

INFLUX_HOST = 'localhost'
INFLUX_PORT = 8086
INFLUX_DB_NAME = 'hytech_telemetry'

TIMEZONE = 0

def influx_connect():
    while True:
        print('Attempting to connect to database at {}:{}'.format(INFLUX_HOST, INFLUX_PORT))
        try:
            client = InfluxDBClient(host=INFLUX_HOST, port=INFLUX_PORT)
            db_exists = False
            for db in client.get_list_database():
                if db['name'] == INFLUX_DB_NAME:
                    db_exists = True
                    client.switch_database(INFLUX_DB_NAME)
                    break
            if not db_exists:
                requests.get('http://{}:{}/query?q=CREATE+DATABASE+"{}"'.format(INFLUX_HOST, INFLUX_PORT, INFLUX_DB_NAME))
                client.switch_database(INFLUX_DB_NAME)
            break
        except Exception as e:
            print("Influx connection refused. Trying again in ten seconds.")
            time.sleep(10)
    print("Connected using database {}".format(INFLUX_DB_NAME))
    return client


def mqtt_connect(client, userdata, flags, rc):
    client.subscribe(MQTT_TOPIC)
    print("Subscribed to", MQTT_TOPIC)
    client.publish(MQTT_TOPIC, "Python client connected")

# id_map = [False] * 256;

def mqtt_message(client, userdata, msg):
    timestamp = msg.payload[0:msg.payload.find(b',')]
    frame = msg.payload[msg.payload.find(b',') + 1:-1]
    frame = binascii.hexlify(frame)
    s = ""
    for c in frame:
        s += chr(c)
    # print(s)
    data = unpack(s)
    # print(data)

    if data != -1:
        timestamp = int(timestamp.decode()) + 946684800000 + TIMEZONE #  LTE Module stuck in 1990. Fix if you can.
        json_body = []
        for readout in decode(data):
            if len(str(readout[1])) == 0:
                continue
            if len(readout) == 2:
                json_body.append({
                    "measurement": readout[0],
                    "time": timestamp,
                    "fields": {
                        "value": readout[1]
                    }
                })
            else:
                json_body.append({
                    "measurement": readout[0],
                    "time": timestamp,
                    "fields": {
                        "value": readout[1],
                        "units": readout[2]
                    }
                })
            # if (not id_map[data[0]]):
                # print (hex(data[0]).upper())
                # id_map[data[0]] = True
        # print("Writing document: ")
        # print(json_body)
        try:
            influx_client.write_points(points=json_body, time_precision='ms')
        except Exception as e:
            print("Operation failed. Printing error:")
            print(e)

def tz_connect(client, userdata, flags, rc):
    client.subscribe('hytech_car/timezone_registration')

def tz_message(client, userdata, msg):
    global TIMEZONE
    TIMEZONE = (int(time.time()) - (int(msg.payload.decode()) + 946684800)) // 3600 * 3600000

def live():
    # Set up mqtt connection
    tz_client = mqtt.Client()
    tz_client.connect(MQTT_SERVER, MQTT_PORT, 60)
    tz_client.on_connect = tz_connect
    tz_client.on_message = tz_message
    tz_client.loop_start()

    mqtt_client = mqtt.Client()
    mqtt_client.connect(MQTT_SERVER, MQTT_PORT, 60)
    mqtt_client.on_connect = mqtt_connect
    mqtt_client.on_message = mqtt_message
    mqtt_client.loop_start()
    print("Loop start")

    threading.Event().wait()
    # Wait for q to quit
    # input()

    # Time to quit, disconnect MQTT
    # Time to quit, disconnect MQTT
    # print("Loop stop")
    # client.loop_stop()
    # client.disconnect() # TODO unsure if this should be called

def unpack(frame):
    # print("----------------")
    frame = ''.join(char for char in frame if char.isalnum())
    if (len(frame) != 32):
        # TODO throw an error up on screen
        # print("Malformed frame len " + str(len(frame)) + " encountered - skipping")
        return -1
    '''frameprint = ''
    odd = False
    for char in frame:
        frameprint += char
        if odd:
            frameprint += " "
        odd = not odd
    print("Encoded frame: " + frameprint.upper())'''
    try:
        decoded = cobs.decode(binascii.unhexlify(frame))
    except Exception as e:
        print("Decode failed: " + str(e))
        return -1
    # Calculate checksum
    checksum = fletcher16(decoded[0:13])
    cs_calc = chr(checksum >> 8) + " " + chr(checksum & 0xFF)
    cs_rcvd = chr(decoded[14]) + " " + chr(decoded[13])
    if cs_calc != cs_rcvd:
        # print("Decode failed: Checksum mismatch - calc: " + cs_calc + " - rcvd: " + cs_rcvd)
        return -1
    '''out = "Decoded frame: "
    for char in decoded:
        out += binascii.hexlify(char).upper() + " "
    print(out)'''
    return decoded

def fletcher16(data):
    d = data # map(ord,data)
    index = 0
    c0 = 0
    c1 = 0
    i = 0
    length = len(d)
    while length >= 5802:
        for i in range(5802):
            c0 += d[index]
            c1 += c0
            index += 1
        c0 %= 255
        c1 %= 255
        length -= 5802

    index = 0
    for i in range(len(data)):
        c0 += d[index]
        c1 += c0
        index += 1
    c0 %= 255
    c1 %= 255
    return (c1 << 8 | c0)

influx_client = influx_connect()
live()
