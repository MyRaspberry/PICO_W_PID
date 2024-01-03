// settings.h
// ip format is tricky, array of bytes so IPAddress thisip(FIXIP) understands it

#ifndef STASSID
#define STASSID "kll-wlan_2.4G"
#define STAPSK "WELOVEKOH-SAMUI"
#define REV "v1.0.0"
#define FIXIP {192,168,1,215}
#define PORT 1234
// here use old broker setup for PICO W
#define MQTT_broker {192,168,1,104}
#define MQTT_port 1883
#define MQTT_user "uPICOW"
#define MQTT_pass "pPICOW"
#define MQTT_mtopic "PICOW"
#define MQTT_dtopic "PID/P215"

#endif
