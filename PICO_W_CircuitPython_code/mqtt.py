# expect /lib/adafruit_minimqtt/
import gc
Imp=(f"\nFREE MEM report from mqtt.py after imports\n+ import gc {gc.mem_free()}\n")
import os
Imp+=(f"+ import os {gc.mem_free()}\n")
import time
Imp+=(f"+ import time {gc.mem_free()}\n")
#   from adafruit_datetime import  datetime
#   Imp+=(f"+ from adafruit_datetime import  datetime {gc.mem_free()}\n")
import adafruit_minimqtt.adafruit_minimqtt as MQTT
Imp+=(f"+ import adafruit_minimqtt.adafruit_minimqtt as MQTT {gc.mem_free()}\n")
from pico_w_io import get_PV, get_SP, get_OUT, get_pid_details, get_pid_mode, get_T0val

# _________________________________________________________ expect file tools.py
from tools import DIAG, DIAGM, dp, check_mem

if ( DIAGM ) : print(Imp)
del Imp # ________________________________________________ variable needed for boot only

useNTP = bool(os.getenv('useNTP')) # send timestamp

def time_now(): # ______________________________________ use time and NOT datetime
    if useNTP :
        now = time.localtime()
        nows = f"{now.tm_year}-{now.tm_mon:02}-{now.tm_mday:02} {now.tm_hour:02}:{now.tm_min:02}:{now.tm_sec:02}"
        return nows
    else:
        return ""


MQTT_count = 0
MQTT_counts = '%d' % MQTT_count
mqtts=""

useMQTT = False  # ___________________________ used to setup MQTT broker connection

def mqtt_connect(pool) : # ________________________ use pool from web_wifi.py in setup_webserver what calles this
    global MQTTok, useMQTT, mqtt_client, MQTT_count, MQTT_dtopic, mqtt_topic
    useMQTT = bool(os.getenv('useMQTT'))
    if ( useMQTT ) :
        dp("mqtt try connect")
        MQTTok = False
        MQTT_broker = os.getenv('MQTT_broker') # ___ RPI IP
        MQTT_port = os.getenv('MQTT_port') # __ 1883
        MQTT_user = os.getenv('MQTT_user') # _______ u213
        MQTT_pass = os.getenv('MQTT_pass') # _______ p213
        MQTT_TLS = os.getenv('MQTT_TLS') # _________ "n"
        MQTT_mtopic = os.getenv('MQTT_mtopic')
        MQTT_dtopic = os.getenv('MQTT_dtopic')

        MQTT_count = 0 # _____________________________________ send to broker as id too
        mqtt_init="Hello Broker: i am "
        mqtt_topic = MQTT_mtopic + "/" + MQTT_dtopic # like

        def connect(mqtt_client, userdata, flags, rc):
            # This function will be called when the mqtt_client is connected
            # successfully to the broker.
            dp("mqtt Connected to MQTT Broker!")
            dp("mqtt Flags: {0} RC: {1}".format(flags, rc))

        def disconnect(mqtt_client, userdata, rc):
            # This method is called when the mqtt_client disconnects
            # from the broker.
            dp("mqtt Disconnected from MQTT Broker!")

        def subscribe(mqtt_client, userdata, topic, granted_qos):
            # This method is called when the mqtt_client subscribes to a new feed.
            dp("mqtt Subscribed to {0} with QOS level {1}".format(topic, granted_qos))

        def unsubscribe(mqtt_client, userdata, topic, pid):
            # This method is called when the mqtt_client unsubscribes from a feed.
            dp("mqtt Unsubscribed from {0} with PID {1}".format(topic, pid))

        def publish(mqtt_client, userdata, topic, pid):
            # This method is called when the mqtt_client publishes data to a feed.
            dp("mqtt Published to {0} with PID {1} ".format(topic, pid))

        def message(client, topic, message):
            dp("mqtt New message on topic {0}: {1}".format(topic, message))

        dp("mqtt ++ setup MQTTclient")
        dp("mqtt ++ NO TLS")

        mqtt_client = MQTT.MQTT(
            broker=MQTT_broker,
            port=MQTT_port,
            username=MQTT_user,
            password=MQTT_pass,
            socket_pool=pool
            #ssl_context=ssl.create_default_context(),
            )

        # Connect callback handlers to mqtt_client
        mqtt_client.on_connect = connect
        mqtt_client.on_disconnect = disconnect
        mqtt_client.on_subscribe = subscribe
        mqtt_client.on_unsubscribe = unsubscribe
        mqtt_client.on_publish = publish
        mqtt_client.on_message = message

        dp("mqtt broker: "+MQTT_broker)
        dp("mqtt topic: "+mqtt_topic)

        try:
            dp("mqtt Attempting to connect to %s" % mqtt_client.broker)
            mqtt_client.connect()
            MQTTok = True # _______________________________ used later for publish
        except Exception as e:
            print("Error: MQTT connect\n", str(e))
            MQTTok = False

        try:
            dp("mqtt Publishing to topic: %s" % MQTT_mtopic) # ____ only master topic gets that
            mqtt_hello = mqtt_init+mqtt_topic
            dp(mqtt_hello)
            mqtt_client.publish(MQTT_mtopic,mqtt_hello )
            MQTTok = True # _______________________________ used later for publish
        except Exception as e:
            print("Error: MQTT publish hello\n", str(e))
            #MQTTok = False


# ______________________here construct a MQTT JSON like string for the data, escape the { with {{
MQTT_JSON_string = """{{ \"id\": {MQTT_counts}, \"dev\":\"{MQTT_dtopic}\", \"datetimes\": \"{tnows}\", \"PS_Temp\": {MTemps:,.2f}, \"PID_PV\": {PVs:,.2f}, \"PID_SP\": {SPs:,.2f}, \"PID_OUT": {OUTs:,.2f}, \"PID_MODE\": {MODEs} }}"""

def mqtt_send(): # ________________________________________ from code JOB2
    global MQTT_count, MQTT_counts, mqtt_client, MQTT_dtopic, mqtt_topic, MQTTok, mqtts

    tnows = time_now()

    MQTT_counts = '%d' % MQTT_count
    #dp(MQTT_counts)

    def get_MODEs():
        isMODE = get_pid_mode()
        if isMODE : # for trending in NodeRed need a number: 0 LOCK, 1 MAN, 2 AUTO, 3 RSP
            return "2"
        else:
            return "1"


    mqtts = MQTT_JSON_string.format(
        MQTT_counts=MQTT_counts,
        MQTT_dtopic=MQTT_dtopic,
        tnows=tnows,
        MTemps=get_T0val(),
        PVs=get_PV(),
        SPs=get_SP(),
        OUTs=get_OUT(),
        MODEs=get_MODEs(),
    )
    dp(mqtts)
    if MQTTok :
        mqtt_client.publish(mqtt_topic, mqtts )
    MQTT_count += 1 # for next loop

def get_mqtts(): # ______________________________________________ show also in web page
    return mqtts

#RPI4 mqttsub see:
"""
PICOW Hello Broker: i am PICOW/PID/P215
PICOW/PID/P215
{ "id": 0, "dev":"PID/P215", "datetimes": "2023-12-13 19:05:20", "PS_Temp": 33.69, "PID_PV": 0.49, "PID_SP": 5.00, "PID_OUT": 25.47, "PID_MODE": 2 }

PICOW/PID/P215
{ "id": 1, "dev":"PID/P215", "datetimes": "2023-12-13 19:06:20", "PS_Temp": 33.69, "PID_PV": 0.61, "PID_SP": 5.00, "PID_OUT": 27.53, "PID_MODE": 2 }

"""

