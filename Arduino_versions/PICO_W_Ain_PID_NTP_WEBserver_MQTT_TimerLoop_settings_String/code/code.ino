/*

   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

  start from example AdvancedWebServer KLL 12/2023
  rev use STRING and less CHAR's

*/

String A_Reads = " A_Reads "; // feed by pico_w_io
String Tnows = " Tnows "; // feed by common
String MQTTs = " MQTTs "; // feed by web_wifi
char MQTTc[200];
char Tnowc[100];

void setup(void) {
  delay(500);
  Serial.begin(115200); // __________________________________________ talk to serial
  delay(500);
  Serial.println();
  Serial.println("PICO W on Arduino IDE 2.2.1"); // _______________ hooray, now i see it!

  set_IO(); // ____________________________________________________ in pico_w_io
  setup_WIFI(); // __________________________________________________ see web_wifi.ino
  setup_MQTT(); // ____________________________________________________ see web_wifi.ino
}

int loopc = 0;
const int loopclim = 1000; // _______________________________________ cycles until we check time
int thissec = 0;
int thismin = 0;

void loopC () {
  loopc += 1;
  if ( loopc >= loopclim ) {
    loopc = 0;
    time_t now = time(nullptr);
    struct tm ti;
    gmtime_r(&now, &ti);
    int nowsec = ti.tm_sec;
    if (nowsec != thissec) {
      thissec = nowsec;
      thismin = ti.tm_min;
      Serial.print(".");
      //Serial.println(thissec); // _________________________________ other 1 sec JOBs
      //DO1ramp(); // _______________________________________________ TEST PWM
      run_WIFI(); // ________________________________________________ moved from main
      run_MQTT(); // ________________________________________________ also from web_wifi.ino ( for subscription ... )

    }
  }
}

int loopNs = 0;
int loopNc = 0;
int loopNslim = 15; // ______________________________________________ make a 15 sec tick

void JobNsec() { // _________________________________________________ and do some analog reading
  Serial.println(); // ______________________________________________ after the "." need a LF
  Ains(); // ________________________________________________________ do the readings see pico_w_io.ino AND PID and store to global
  Serial.println(A_Reads);
}

void loopNsec() {
  if (loopNs != thissec) {
    loopNs = thissec;
    loopNc += 1;
    if ( loopNc >= loopNslim ) {
      loopNc = 0;
      JobNsec();
    }
  }
}

int loop1minc = 0;

void Job1min(){
  Serial.println();
  Serial.print("___ loop1min ");
  nows();
  Serial.println(Tnows); // set by nows();
  make_MQTT(); // only makes the string
  send_MQTT();
}

void loop1min() { // ________________________________________________ you will see at startup it is executed, and also at the first minute change, but then dt is 1min
  if ( loop1minc != thismin ) {
    loop1minc = thismin;
    Job1min();
  }
}

int loop1M = 0;
unsigned long lastTms = millis();
unsigned long nowms=lastTms;
bool showms = false; // true;
bool showT = false; // true;

void loopM() {
  loop1M += 1;
  if ( loop1M >= 1000000 ) { // _____________________________________ about 11 sec ? now 22 sec / without MDNBS.update now 9 sec
    loop1M = 0;
    if ( showms ) {
      nowms = millis();
      Serial.printf("\n___ loop1M dt : %.2f",(nowms - lastTms)/1000.0);
      lastTms = nowms;
    }
    if ( showT ) {
      Serial.println(); // ____________________________________________ after the "." need a LF
      Serial.print("___ loop1M: ");
      nows();
      Serial.println(Tnows); // set by nows();
    }
  }
}

void loop(void) {
  loopC();
  loopNsec();
  loop1min();
  loopM();
}

/*
this is with a PICO W from Arduino 2.2.1
set :
showms = true

loopM only 
___ loop1M dt : 0.64
loopM & loopC
___ loop1M dt : 0.82
loopM & loopC & loopNsec ( that calls ains and PID and DO1 )
___ loop1M dt : 0.99
loopM & loopC & loopNsec & loop1min
___ loop1M dt : 1.15
loopM & loopC & loopNsec & loop1min & run_WIFI() WITH 2 browser index and data OPEN auto refresh
___ loop1M dt : 9.58

loopM & loopC call run_WIFI() every second only & loopNsec & loop1min & WITH 2 browser index and data OPEN auto refresh
___ loop1M dt : 1.18


this is with a ESP32-S3 from Arduino 2.2.1 ?? there is something in the background stalling?
loopM only 
___ loop1M dt : 5.68
loopM & loopC & call run_WIFI() every second only
___ loop1M dt : 5.77
loopM & loopC & loopNsec ( that calls ains and PID and DO1 )
___ loop1M dt : 5.85
loopM & loopC call run_WIFI() every second only & loopNsec & loop1min & WITH 2 browser index and data OPEN auto refresh
___ loop1M dt : 5.94

with MQTT and MQTT sub and RGB LED drive MQTT remote 
___ loop1M dt : 6.03
*/