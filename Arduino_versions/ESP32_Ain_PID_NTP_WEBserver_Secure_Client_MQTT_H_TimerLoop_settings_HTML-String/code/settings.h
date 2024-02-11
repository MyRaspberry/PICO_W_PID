// settings.h
// __________________________________________________________________ use switches to select BROKER ( only ONE )
//#define useREMOTE_BROKER_EMQX
#define useREMOTE_BROKER_HIVEMQ
//#define useLOCAL_BROKER


#ifndef STASSID
#define STASSID "___SSID___"
#define STAPSK "___PASS___"
#define REV "v1.1.0"
#define A_IDE " on Arduino IDE 2.3.0"
#define FIXIP {192,168,1,215}
#define PORT 1234

#define MQTT_mtopic "PICOW"
#define MQTT_dtopic "PID/P215"

// __________________________________________________________________ REMOTE WITH TLS

#ifdef useREMOTE_BROKER_EMQX

#define MQTT_HOST "broker.emqx.io"        // Broker address
#define MQTT_SECURE true
#define MQTT_PORT 8883
#define useCREDETIALS false
const uint8_t MQTT_SERVER_FINGERPRINT[] = {0x7e, 0x36, 0x22, 0x01, 0xf9, 0x7e, 0x99, 0x2f, 0xc5, 0xdb, 0x3d, 0xbe, 0xac, 0x48, 0x67, 0x5b, 0x5d, 0x47, 0x94, 0xd2};

#endif

// __________________________________________________________________ REMOTE WITH TLS

#ifdef useREMOTE_BROKER_HIVEMQ

#define MQTT_HOST "xxx.s2.eu.hivemq.cloud"        // OK
#define MQTT_SECURE true
#define MQTT_PORT 8883
#define useCREDETIALS true
#define MQTT_user "___USER___"
#define MQTT_pass "___PASS___"

#endif

// __________________________________________________________________ LOCAL NO TLS

#ifdef useLOCAL_BROKER

#define MQTT_HOST "192.168.1.104"                                              // OK
#define MQTT_SECURE false
#define MQTT_PORT 1883
#define useCREDETIALS true
#define MQTT_user "uPICOW"
#define MQTT_pass "pPICOW"

#endif

#endif
