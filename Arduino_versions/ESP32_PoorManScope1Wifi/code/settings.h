// settings.h
// ip format is tricky, array of bytes so IPAddress thisip(FIXIP) understands it

#ifndef STASSID
#define STASSID "___SSID___"
#define STAPSK "___PASS___"
#define REV "v1.0.0"
#define FIXIP {192,168,1,215}
#define PORT 1234
// optional pwm output to test osci jumper pins 
#define TESTPWM 1

#endif
