/*
  start from example AdvancedWebServer KLL 12/2023
  revision from: 
  ESP32_Ain_PID_NTP_WEBserver_MQTT_TimerLoop_settings_String
  to:
    ESP32_Ain_PID_NTP_WEBserver_Secure_Client_MQTT_H_TimerLoop_settings_HTML-String
  !now with MQTT TLS by MQTT.h, WiFiClientSecure and NO CERT
*/

#include "settings.h" // ______________________________________ here find the router login SSID PASSWORD ...

SET_LOOP_TASK_STACK_SIZE(16*1024); // 16KB //still Stack smashing protect failure!

String A_Reads = " A_Reads "; // feed by esp32_io
String Tnows = " Tnows "; // feed by common
String MQTTs = " MQTTs "; // feed by web_wifi
char MQTTc[200];
char Tnowc[100];

void setup(void) {
  delay(500);
  Serial.begin(115200); // __________________________________________ talk to serial
  // this blocks if on CHARGER only //while (!Serial) { delay(500); }
  delay(500);
  Serial.println();

  Serial.print(ARDUINO_BOARD);
  Serial.println(A_IDE); // _______________ ESP32S3_DEV on Arduino IDE 2.3.0

  Serial.printf("Arduino Stack was set to %d bytes", getArduinoLoopTaskStackSize());
  Serial.printf("\nSetup() - Free Stack Space: %d", uxTaskGetStackHighWaterMark(NULL)); // Print unused stack for the task that is running setup()
  set_IO(); // ______________________________________________________ in esp32_io.ino
  setup_WIFI(); // __________________________________________________ see web_wifi.ino
  setup_MQTT(); // __________________________________________________ see web_wifi.ino
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
      //DO5ramp(); // _______________________________________________ TEST PWM
      run_WIFI(); // ________________________________________________ moved from main
      run_MQTT(); // ________________________________________________ also from web_wifi.ino ( for subscription ... )
      get_RGB_set(); // _____________________________________________ if like remote mqtt set RGB
    }
  }
}

int loopNs = 0;
int loopNc = 0;
int loopNslim = 15; // ______________________________________________ make a 15 sec tick

void JobNsec() { // _________________________________________________ and do some analog reading
  Serial.println(); // ______________________________________________ after the "." need a LF
  Ains(); // ________________________________________________________ do the readings see esp32_io.ino AND PID
  //strncpy(A_Reads,get_Ains(),100 ); // ___________________________________ and get them here as string
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
      Serial.printf("\n___ Loop() - Free Stack Space: %d", uxTaskGetStackHighWaterMark(NULL));
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
