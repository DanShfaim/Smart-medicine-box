import schedule
import time
import datetime
import paho.mqtt.client as mqtt
import _thread
import firebase_admin
from firebase_admin import credentials
from firebase_admin import db


# Define event callbacks
def on_connect(client, userdata, flags, rc):
    # print("rc: " + str(rc))
    print('Server is Running ...')


def on_message(client, obj, msg):
    # print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload.decode()))
    if str(msg.topic) == 'start' and str(msg.payload.decode()) == '1':  # 1 = lets start
        print('Setting Schedule..')
        _thread.start_new_thread(setSchedule,())
    if str(msg.topic) == 'taken':
        msg = ('Med Taken from cell #' + str(msg.payload.decode()[0]) + ' at: '
              + datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S'))
        print(msg)
        with open("Data.txt", "a") as dataFile:
            dataFile.write(msg+'\n')

def on_publish(client, obj, mid):
    # print("mid: " + str(mid))
    return


def on_subscribe(client, obj, mid, granted_qos):
    # print("Subscribed: " + str(mid) + " " + str(granted_qos))
    return


def on_log(client, obj, level, string):
    print(string)


def sendMSG(msg):
    # Publish a message
    mqttc.publish(topic1, msg)
    msg = ('Led on cell number #' + msg + ' turns on at: '
              + datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S'))
    print(msg)
    with open("Data.txt", "a") as dataFile:
        dataFile.write(msg+'\n')


def setSchedule():
    cred = credentials.Certificate('keyFile.json')
    firebase_admin.initialize_app(cred, {
        'databaseURL': 'https://medicineapp1-e74f6.firebaseio.com/'
    })
    jday = db.reference('users')
    jday = jday.get()['0506996055']
    for day in jday['days']:
        curDay = day[:3]
        curTime = jday['days'][day]
        if curDay == 'Sun':
            schedule.every().sunday.at(curTime).do(sendMSG, '4')
        elif curDay == 'Mon':
            schedule.every().monday.at(curTime).do(sendMSG, '2')
        elif curDay == 'Tue':
            schedule.every().tuesday.at(curTime).do(sendMSG, '4')
        elif curDay == 'Wed':
            schedule.every().wednesday.at(curTime).do(sendMSG, '4')
        elif curDay == 'Thu':
            schedule.every().thursday.at(curTime).do(sendMSG, '5')
        elif curDay == 'Fri':
            schedule.every().friday.at(curTime).do(sendMSG, '6')
        elif curDay == 'Sat':
            schedule.every().saturday.at(curTime).do(sendMSG, '4')

    while True:
        schedule.run_pending()
        time.sleep(60) # wait one minute


if __name__ == "__main__":
    mqttc = mqtt.Client()
    # Assign event callbacks
    mqttc.on_message = on_message
    mqttc.on_connect = on_connect
    mqttc.on_publish = on_publish
    mqttc.on_subscribe = on_subscribe

    # Uncomment to enable debug messages
    #mqttc.on_log = on_log

    topic1 = 'OpenCell'
    topic2 = 'start'
    topic3 = 'taken'

    # Connect
    mqttc.username_pw_set('umtyzxvr', 'MeHesYzh-O8m')
    mqttc.connect('m23.cloudmqtt.com', 11608)


    # Start subscribe, with QoS level 0
    mqttc.subscribe(topic2, 0)
    mqttc.subscribe(topic3, 0)

    # Continue the network loop, exit when an error occurs
    rc = 0
    while rc == 0:
        rc = mqttc.loop()
    print("rc: " + str(rc))
