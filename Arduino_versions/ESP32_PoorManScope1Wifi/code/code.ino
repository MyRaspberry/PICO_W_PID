// code.ino
#include <esp_adc_cal.h>
#include <esp32-hal-adc.h>

SET_LOOP_TASK_STACK_SIZE(16*1024); // 16KB //still Stack smashing protect failure!

// YD-ESP32-S3 on board RGB LED NeoPixel W2812 solder RGB jumper, talk to GPIO48
#define RGBLED_PIN 48
#define MAX_BRIGHT 127  // 0 .. 255

byte esp_cR=0; //MAX_BRIGHT;
byte esp_cG=0;
byte esp_cB=MAX_BRIGHT;

#define A0pin 1 // __________ GPIO1 (1) ADC1_0 pin 4 right
int sensorA0 = A0pin;   // select the input pin for the potentiometer // PMS1W input


float in_pct(int ain) {
  float ainr = 100.0 * ain / 4095.0;
  return ainr;
}

float in_volt(int ain) {
  float ainr = 3.3 * ain / 4095.0;
  return ainr;
}

#include <Esp.h>
#include <time.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET     0
#define UTC_OFFSET_DST 0

void setup_NTP() { // _______________________________________________ used in wifi setup
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
}


int TZs = 7*3600; // ____________________________________________________ add TZ, Time zone in Thailand (GMT+7) 

String Tnows = " Tnows "; // feed by common
char Tnowc[100];

// use : nows(); Serial.println(Tnows);
void nows() { 
  time_t now = time(nullptr);
  now += TZs; // ____________________________________________________ add TZ, Time zone in Thailand (GMT+7) 
  struct tm ti;
  gmtime_r(&now, &ti);
  sprintf(Tnowc, "%d-%02d-%02d %02d:%02d:%02d",ti.tm_year+1900,ti.tm_mon+1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec );
  Tnows = String(Tnowc);
}

void show_mem() {
  Serial.println("[APP] Total memory: " + String(ESP.getHeapSize()) + " bytes");
  Serial.println("[APP] Free memory: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("This task watermark: " + String(uxTaskGetStackHighWaterMark(NULL)) + " bytes");
}

#define BIGbuf 20000
char DYN_HTML[BIGbuf];
String DYN_HTMLs = " DYN_HTMLs "; // try for creation of HTML a string type, only before send convert back to char

#include "settings.h" // ______________________________________ here find the router login SSID PASSWORD ...
const char *ssid = STASSID;
const char *password = STAPSK;
const char *rev = REV;
const int port = PORT;
IPAddress thisip(FIXIP); // ___________________________________ fix IP for webserver

// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

bool TestPWM = TESTPWM;

void setClock() {
  setup_NTP();
}

WebServer server(port);

String HtmlStyle() { // reusable page formatting
  String Style = "<style>\
  body {font-family: \"Times New Roman\", Times, serif; background-color: lightgreen;\
  display:inline-block; margin: 0px auto; text-align: center;}\
  h1 {color: deeppink; word-wrap: break-word; padding: 1vh; font-size: 30px;}\
  p {font-size: 1.5rem; word-wrap: break-word;}\
  p.dotted {margin: auto; width: 75%; font-size: 25px; text-align: center;}\
  form {font-size: 2rem; }\
  input[type=number] {font-size: 2rem;}\
  table { width: 100%; }\
  </style>";
  return Style;
}

String button(String txt="text") {
  String svg =   "<svg width=\"110\" height=\"55\" xmlns=\"http://www.w3.org/2000/svg\">";
  svg +=  "<style>.button2 { fill: #007bbf;  cursor: pointer; }";
  svg +=  ".button2:hover { fill: #006919; }";
  svg +=  "</style>";
  svg +=  "<g onmouseup=\"buttonClick(evt)\">";
  svg +=  "<rect class = \"button2\" x=\"20\" y=\"1\" rx=\"5\" ry=\"5\" width=\"90\" height=\"50\"/>";
  svg +=  "<text x=\"38\" y=\"35\" font-size=\"1.8em\">";
  svg +=  txt;
  svg +=  "</text></g></svg>";
  return svg;
}

int rates[] = {50000,10000,5000,1000,500,100};    // called sample rate
String buttontxt[] = {"50k","10k"," _5k"," _1k"," 0.5k"," 0.1k"};

String html_speed_buttons() {
  // need operation buttons for use different rates
  String htmldata = "<div><hr>";
  htmldata += "<table align=\"center\"><tr><td width=\"10%\"></td></tr>\n";
  htmldata += "<tr>";

  for ( int i=0; i < 6; i++ ) {
    htmldata += "<td width=\"10%\">";
    htmldata += "<a href=\"/osci";
    htmldata += String(i);
    htmldata += "\" style=\"display: block;\">";  //___ html link with svg button
    htmldata += button( buttontxt[i] );
    htmldata += "</a>";
    htmldata += "</td>";
  }
  htmldata += "</tr></table>\n";
  htmldata += "<hr></div>";
  return htmldata;
}

void handleDetails() {
  Serial.println();
  Serial.print("www osci_details served");
  DYN_HTMLs =  "<!DOCTYPE html><html><head><title>KLL Web Server OSCI detail</title>";
  DYN_HTMLs += HtmlStyle();
  DYN_HTMLs += "</head><body>";
  DYN_HTMLs += "<div><p> <b> measure Analog A0 pin1 to array</b> </p> <p> pls select sample rate </p>";
  DYN_HTMLs += html_speed_buttons();
  DYN_HTMLs += "</div></div></body></html>";
  DYN_HTMLs.toCharArray(DYN_HTML,BIGbuf);
  //Serial.println(DYN_HTML);
  Serial.printf(" len osci_details : %d \n",strlen(DYN_HTML));
  server.send(200, "text/html", DYN_HTML);
  DYN_HTML[0]= '\0'; // kind of clear

}

#define ALONG 320
int arrylong = ALONG;
int osci[ALONG];
int many = arrylong;
unsigned long msstart;
unsigned long msstop;
float thisdt = 0.0;
float thissampletime = 0.0;
float thisHz = 0.0 ;

int delays[] = {0,78,180,900,2000,9000};

void get_OSCI(int ratei=0) {
  msstart=micros();
  if ( ratei == 0 ) {
    for ( int i=0;i<arrylong;i++) {
      //osci[i] = analogRead(sensorA0);  // write to global array
      osci[i] = adc1_get_raw(ADC1_CHANNEL_0); //GPIO1
    }
  }
  else {
    for ( int i=0;i<arrylong;i++) {
      osci[i] = adc1_get_raw(ADC1_CHANNEL_0); //GPIO1
      delayMicroseconds(delays[ratei]);
    }
  }

  msstop=micros();
  thisdt = (msstop-msstart)/1000.0; // aka millis float
  thissampletime = thisdt/arrylong;
  thisHz = 1000.0/thissampletime;
}

void html_osci_page(int ratei = 0) {
  //global  thisdt, thisHz, thissampletime from get_OSCI()
  Serial.print("\nwww osci details served ");
  get_OSCI(ratei); // _________________________ returns a array with 320 values of 0 .. 4096 of A0 to global osci array
  many = arrylong; //len(osci);

  DYN_HTMLs = "<!DOCTYPE html><html><head><title>KLL OSCI - PMS1W</title>";
  DYN_HTMLs += HtmlStyle();
  DYN_HTMLs += "</head><body>";
  DYN_HTMLs += "<div><p> <b> measure Analog A0 to array</b> </br></br> no autorefresh / sampling at YOUR manual browser refresh</br>or use below menu to change sample rate</p>";
  DYN_HTMLs += " <hr><p>";
  DYN_HTMLs += String(many);
  DYN_HTMLs += " samples in ";
  DYN_HTMLs += String(thisdt,2);
  DYN_HTMLs += " millis, </br>that is ";
  DYN_HTMLs += String(thissampletime,3);
  DYN_HTMLs += " millis sample time or ";
  DYN_HTMLs += String(thisHz,1);
  DYN_HTMLs += " Hz </br>called: ";
  DYN_HTMLs += String(rates[ratei]);
  DYN_HTMLs += " rate,</p>";

  int smin = 4096; // from a 12 bit ADC
  int smax = 0;
  for ( int i = 0; i < many; i++) {
    //osci[i] = int( osci[i]/16 ); // _______________ new range from bufio back to 4096;
    if ( osci[i] < smin ) { smin = osci[i]; }
    if ( osci[i] > smax ) { smax = osci[i]; }
  }  //

  DYN_HTMLs +=  "<p> MIN : ";
  DYN_HTMLs +=  String(smin);
  DYN_HTMLs +=  ", MAX : ";
  DYN_HTMLs +=  String(smax);
  DYN_HTMLs +=  " of 4096 aka 3.3 Volt</p>";

  DYN_HTMLs +=  "<table align=\"center\"><tr><td width=\"10%\"></td><td width=\"10%\"></td><td width=\"10%\"></td></tr>\n";
  DYN_HTMLs +=  "<tr><td width=\"10%\"></td><td>\n";

  // we measure to a array over 320 samples the numbers are Volt at the A0 pin ( 0 .. 3.3 ) as 1023? 4096.
  // our plot area is 640 * 640 in a 660 * 670 svg rectangle, here try the math for it
  int scopew = 640;
  int svgw = scopew+20;
  int scopeh = 640;
  int svgh = scopeh+30;

  DYN_HTMLs += "<svg width=\"";
  DYN_HTMLs += String(svgw);
  DYN_HTMLs += "\" height=\"";
  DYN_HTMLs += String(svgh);
  DYN_HTMLs += "\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">";
  
  DYN_HTMLs += "<desc>show trend line ";
  DYN_HTMLs += String(many);
  DYN_HTMLs += " data samples </desc>";

  DYN_HTMLs += "<!-- grey box with 3D style blue border-->";
  DYN_HTMLs += "<rect x=\"1\" y=\"1\" width=\"";
  DYN_HTMLs += String(svgw);
  DYN_HTMLs += "\" height=\"";
  DYN_HTMLs += String(svgh);
  DYN_HTMLs += "\"  stroke=\"#000088\" stroke-width=\"1\" />";

  DYN_HTMLs += "<rect x=\"2\" y=\"2\" width=\"";
  DYN_HTMLs += String((svgw-4));
  DYN_HTMLs += "\" height=\"";
  DYN_HTMLs += String((svgh-4));
  DYN_HTMLs += "\"  stroke=\"#0000aa\" stroke-width=\"1\" />";

  DYN_HTMLs += "<rect x=\"3\" y=\"3\" width=\"";
  DYN_HTMLs += String((svgw-6));
  DYN_HTMLs += "\" height=\"";
  DYN_HTMLs += String((svgh-6));
  DYN_HTMLs += "\"  fill=\"#FFF8DC\" stroke=\"#0000ff\" stroke-width=\"1\" />";

  DYN_HTMLs += "<g id=\"trend\" style=\"visibility:visible; stroke:red; stroke-width:2; \" > ";

  int xo = 10;
  int x1 = 0;
  int x2 = 0;
  int yo = 15;
  int y1 = 0;
  int y2 = 0; //_ offset from  top left
  int yf1=0.0;
  int yf2=0.0;

  for ( int xi =0; xi < many-1;xi++) {
    yf1 = osci[xi];
    yf2 = osci[xi + 1];
    x1 = xo + xi*2;
    x2 = x1 + 2;
    y1 = yo + scopeh - int(yf1 *scopeh/4096);
    y2 = yo + scopeh - int(yf2 *scopeh/4096);
    DYN_HTMLs += "<line x1=\"";
    DYN_HTMLs += String(x1);
    DYN_HTMLs += "\" y1=\"";
    DYN_HTMLs += String(y1);
    DYN_HTMLs += "\" x2=\"";
    DYN_HTMLs += String(x2);
    DYN_HTMLs += "\" y2=\"";
    DYN_HTMLs += String(y2);
    DYN_HTMLs += "\" />\n";
  }
  DYN_HTMLs += ("</g>");
  DYN_HTMLs += ("</svg>\n");

  DYN_HTMLs +=  "</td><td width=\"10%\"></td></tr></table></br>\n";
  DYN_HTMLs +=  "</div>";

  //DYN_HTMLs +=  '<hr></br><a href="/osci_details" ><b>osci-speed-details</b></a> </br></br> <hr>';
  DYN_HTMLs += html_speed_buttons();
  DYN_HTMLs +=  "</body></html>";

  if ( DYN_HTMLs.length() > BIGbuf ) {
    Serial.print("\nWARNING ! BIGbuf too small, need ");
    Serial.println(DYN_HTMLs.length());
  }
  DYN_HTMLs.toCharArray(DYN_HTML,BIGbuf);
  //Serial.println(DYN_HTML);
  Serial.printf(" len : %d \n",strlen(DYN_HTML));
  server.send(200, "text/html", DYN_HTML);
  DYN_HTML[0]= '\0'; // kind of clear    
}


void handleRoot() {
  Serial.println();
  Serial.print("www index.html served");
  //LED_toggle();
  // uptime char
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  char uptimec[100];
  snprintf(uptimec,100,"%02d:%02d:%02d",hr, min % 60, sec % 60);

  nows(); // and use global Tnows STRING

  DYN_HTMLs  = "<!DOCTYPE html><html><head>\
  <meta http-equiv='refresh' content='15'/>\
  <title>KLL engineering</title>";
  DYN_HTMLs  += HtmlStyle();
  DYN_HTMLs  += "</head>\
  <body>\
  <h1>ESP32-S3 Webserver</h1> <h3>via Arduino IDE 2.2.1</h3>\
  <img src='https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/_images/ESP32-S3_DevKitC-1_pinlayout_v1.1.jpg' style='margin:15px' alt='esp32-s3' width='500' >\
  <hr>\
  <a href='/osci_details' target='_blank' ><b>osci: PMS1W / <b>P</b>oor<b>M</b>an<b>S</b>cope <b>1</b> channel <b>W</b>IFI</b></a>\
  <hr><table><tr><th>\
  <p><a href='http://kll.byethost7.com/kllfusion01/infusions/articles/articles.php?article_id=230'\
  target='_blank' ><b>kll engineering blog</b></a></p></th><th><p>rev: ";
  DYN_HTMLs  += String(rev);
  DYN_HTMLs  += "</p></th></tr><tr>  <td><p>";
  DYN_HTMLs  += Tnows; // from nows() in common
  DYN_HTMLs  += "</p></td><td><p>Uptime: ";
  DYN_HTMLs  += String(uptimec);
  DYN_HTMLs  += "</p></td>  </tr>  </table>  <hr>  </body></html>";

  DYN_HTMLs.toCharArray(DYN_HTML,BIGbuf); // for HTML back to CHAR, still hope the creation of the string is more save as snprintf ?( about PANIC and Stack smashing protect failure!)
  //Serial.println(DYN_HTML);
  Serial.printf(" len INDEX : %d \n",strlen(DYN_HTML));
  server.send(200, "text/html", DYN_HTML);
  DYN_HTML[0]= '\0'; // kind of clear

  //LED_toggle();
}

void handleNotFound() {
  //LED_toggle();
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  //LED_toggle();
}

void html_osci_page0() {
  html_osci_page(0);
}
void html_osci_page1() {
  html_osci_page(1);
}
void html_osci_page2() {
  html_osci_page(2);
}
void html_osci_page3() {
  html_osci_page(3);
}
void html_osci_page4() {
  html_osci_page(4);
}
void html_osci_page5() {
  html_osci_page(5);
}

void setup_WIFI() {
  // _______________________________________ connect to router
  WiFi.mode(WIFI_STA);
  WiFi.config(thisip, gateway, subnet, primaryDNS, secondaryDNS);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("www Connected to ");
  Serial.println(ssid);
  Serial.print("www IP address: ");
  Serial.println(WiFi.localIP());

  setClock(); // ____________________________ setup async ( 5 secs ) NTP to RTC

  //if (MDNS.begin("esp32")) {
  //  Serial.println("MDNS responder started");
  //}

  server.on("/", handleRoot);
  server.on("/osci_details",handleDetails);
  server.on("/osci0",html_osci_page0 ); //  [FAST] 
  server.on("/osci1",html_osci_page1 ); //  [>>>>] 
  server.on("/osci2",html_osci_page2 ); //  [>>>] 
  server.on("/osci3",html_osci_page3 ); //  [>>] 
  server.on("/osci4",html_osci_page4 ); //  [>] 
  server.on("/osci5",html_osci_page5 ); //  [SLOW] 
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.print("www HTTP server started on http://");
  Serial.print(thisip);
  Serial.print(":");
  Serial.println(port);
}

void run_WIFI() {
  server.handleClient();
}

#define LEDC_CHANNEL_0 0 // _________________________________________ use first channel of 16 channels (started from zero)
#define LEDC_TIMER_12_BIT 12 // _____________________________________ use 12 bit precission for LEDC timer
#define LEDC_BASE_FREQ  1000 // _____________________________________ use 1000 Hz as a LEDC base frequency
#define LED_PIN 6 // ________________________________________________ my board left pin 6 is GPIO06

void setup() {
  delay(500);
  Serial.begin(115200); // __________________________________________ talk to serial
  delay(500);
  Serial.println();
  Serial.println("ESP32-S3 on Arduino IDE 2.2.1"); // _______________ hooray, now i see it!

  Serial.printf("Arduino Stack was set to %d bytes", getArduinoLoopTaskStackSize());
  Serial.printf("\nSetup() - Free Stack Space: %d", uxTaskGetStackHighWaterMark(NULL)); // Print unused stack for the task that is running setup()

  analogReadResolution(12);
  pinMode(1, INPUT);  //GPIO1
  adcAttachPin(1);    //GPIO1

  neopixelWrite(RGBLED_PIN,esp_cR,esp_cG,esp_cB);
  if ( TestPWM ) {
    Serial.println("\n+++ ESP32 TestPWM: GP06 ( here pin 6 left ) use as PWM 'analog' out, use jumper back to OSCI Ain A0 ( here GP01 pin 4 right) ");
    ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT);
    ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);
    ledcWrite(LEDC_CHANNEL_0, 2048); // _____________________ write to LEDC 50% duty
  }

  setup_WIFI(); // __________________________________________________ see web_wifi.ino

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
      run_WIFI(); // ________________________________________________ moved from main
    }
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
  loopM();
}

