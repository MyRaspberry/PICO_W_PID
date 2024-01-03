// web_wifi.ino
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
//#include <LEAmDNS.h>
#include <PubSubClient.h> // old most easy mqtt lib, works NO TLS, local RPI broker .. NO send feedback provided?
#include <ArduinoJson.h> // try to get the mqtt commands to values
StaticJsonDocument<200> Set;

#define BIGbuf 5000
char DYN_HTML[BIGbuf];
String DYN_HTMLs = " DYN_HTMLs "; // try for creation of HTML a string type, only before send convert back to char
// now wait what version is more stable.

// SVG size setup & SVG rectangle shadow color
#define SVG_W 660
#define SVG_4W 656 // -4
#define SVG_6W 654 // -6
#define SVG_H 480
#define SVG_4H 476 // -4
#define SVG_6H 474 // -6

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

IPAddress mqtt_broker(MQTT_broker); // ___________________________________ MQTT IP broker
const int mqtt_port = MQTT_port;
const char *mqtt_user = MQTT_user;
const char *mqtt_pass = MQTT_pass;
const char *mqtt_mtopic = MQTT_mtopic;
const char *mqtt_dtopic = MQTT_dtopic;
char topic[20];
char topicset[25];

byte mqtt_cR=0;
byte mqtt_cG=0;
byte mqtt_cB=0;
bool mqtt_cFlag = false;

byte mqtt_get_cR() {
  return mqtt_cR;
}

byte mqtt_get_cG() {
  return mqtt_cG;
}

byte mqtt_get_cB() {
  return mqtt_cB;
}

bool mqtt_get_cFlag() {
  return mqtt_cFlag;
}

void mqtt_reset_cFlag() {
  mqtt_cFlag = false;
}


WiFiClient picoClient;
PubSubClient client(picoClient);

void setClock() {
  NTP.begin("pool.ntp.org", "time.nist.gov");

  Serial.print("www Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("www Current time: GMT ");
  Serial.print(asctime(&timeinfo)); // 
}

bool MQTT_OK = false; // change because i not want to get it stuck if no broker online BUT now, if start broker later, must reset board

void setup_MQTT(){
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  if ( !MQTT_OK ) {
    String client_id = "picow-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_user, mqtt_pass)) {
      Serial.print("MQTT broker: ");
      Serial.print(mqtt_broker);
      Serial.print(" to topic: ");
      snprintf(topic,20,"%s/%s",mqtt_mtopic,mqtt_dtopic); // ________ make full data topic
      snprintf(topicset,25,"%s/setRGB",topic); // ________ make data topic/set for subscribe ( RGB LED remote operation)
      Serial.print(topic);
      Serial.println(" connected");
      MQTT_OK = true;
      //neopixelWrite(RGBLED_PIN,0,100,0);
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
    }
  }

  if ( MQTT_OK ) {
    // Publish and subscribe
    client.publish(MQTT_mtopic, "I'm a PICO W ! ");
    Serial.print("subscribe to: ");
    Serial.println(topicset);
    client.subscribe(topicset);
  }
}

void callback(char *atopic, byte *apayload, unsigned int length) { // already filtered for PICOW/PID/P215/setRGB
  Serial.print("\nmqtt Message arrived in topic: ");
  Serial.print(atopic);
  Serial.print(" :");
  for (int i = 0; i < length; i++) {
    Serial.print((char) apayload[i]);
  }
  Serial.println();
  
  DeserializationError error = deserializeJson(Set, apayload); // Deserialize the JSON document
  if (error) { // Test if parsing succeeds.
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  mqtt_cR = Set["cR"];
  mqtt_cG = Set["cG"];
  mqtt_cB = Set["cB"];
  mqtt_cFlag = true;
  //RGBshow(cR,cG,cB); // 

}

int MQTT_count = 0;


void make_MQTT() {
  MQTT_count += 1;
  // by job already nows(); // make Tnows
  //snprintf(thisMQTTs,200, "{ \"id\": %d, \"dev\":\"%s\", \"datetimes\": \"%s\", \"PS_Temp\":  %.2f, 
  // <br/> \"PID_PV\": %.2f, \"PID_SP\": %.2f, \"PID_OUT\": %.2f, \"PID_MODE\": %d }"
  // ,MQTT_count,mqtt_dtopic,tnows,get_internalT(),get_PV(),get_SP(),get_OUT(),get_MODE() );
  MQTTs  = "{ \"id\": ";
  MQTTs += String(MQTT_count);
  MQTTs += ", \"dev\":\"";
  MQTTs += String(mqtt_dtopic);
  MQTTs += "\", \"datetimes\": \"";
  MQTTs += Tnows;
  MQTTs += "\", \"PS_Temp\": ";
  MQTTs += String(get_cpu_temp(),1);
  MQTTs += ", \"PID_PV\": ";
  MQTTs += String(get_PV(),2);
  MQTTs += ", \"PID_SP\": ";
  MQTTs += String(get_SP(),2);
  MQTTs += ", \"PID_OUT\": ";
  MQTTs += String(get_OUT(),2);
  MQTTs += ", \"PID_MODE\": ";
  MQTTs += String(get_MODE());
  MQTTs += " }";
  
  Serial.print("___ MQTTstr :\n");
  Serial.println(MQTTs); 
}

void send_MQTT() {
  MQTTs.toCharArray(MQTTc, 200);
  client.publish(topic,MQTTc);
}

void run_MQTT() { // ________________________________________________ only if subscribe to topic
  client.loop();
}

//void send_MQTT(char *topic, char *payload) { // ________________________________________________ only if subscribe to topic
//  client.publish(topic,payload);
//}

WebServer server(port);

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
  <title>KLL engineering</title>\
  <style>\
  body {font-family: \"Times New Roman\", Times, serif; background-color: lightgreen;\
  display:inline-block; margin: 0px auto; text-align: center;}\
  h1 {color: deeppink; word-wrap: break-word; padding: 1vh; font-size: 30px;}\
  p {font-size: 1.5rem; word-wrap: break-word;}\
  p.dotted {margin: auto; width: 75%; font-size: 25px; text-align: center;}\
  form {font-size: 2rem; }\
  input[type=number] {font-size: 2rem;}\
  table { width: 100%; }\
  </style>\
  </head>\
  <body>\
  <h1>PICO W Webserver</h1> <h3>via Arduino IDE 2.2.1</h3>\
  <img src='https://www.raspberrypi.com/documentation/microcontrollers/images/picow-pinout.svg' style='margin:15px' alt='pico_w' width='500' >\
  <hr>\
  <a href='/data' target='_blank' ><b>data</b></a>\
  <hr><table><tr><th>\
  <p><a href='http://kll.byethost7.com/kllfusion01/infusions/articles/articles.php?article_id=230'\
  target='_blank' ><b>kll engineering blog</b></a></p></th><th><p>rev: ";
  DYN_HTMLs  += String(rev);
  DYN_HTMLs  += "</p></th></tr><tr>  <td><p>";
  DYN_HTMLs  += Tnows; // from nows() in common
  DYN_HTMLs  += "</p></td><td><p>Uptime: ";
  DYN_HTMLs  += String(uptimec);
  DYN_HTMLs  += "</p></td>  </tr>  </table>  <hr>  </body></html>";

  DYN_HTMLs.toCharArray(DYN_HTML,5000); // for HTML back to CHAR, still hope the creation of the string is more save as snprintf ?( about PANIC and Stack smashing protect failure!)
  //Serial.println(DYN_HTML);
  Serial.printf(" len INDEX : %d \n",strlen(DYN_HTML));
  server.send(200, "text/html", DYN_HTML);
  DYN_HTML[0]= '\0'; // kind of clear

  //LED_toggle();
}

int get_OUTh(){
  float isOUT = get_OUT();
  return  (400 - int(4*isOUT));
}

int get_PVh() {
  float isPV = get_PV();
  return (400 - int(4*isPV));
}

int get_SPh() {
  float isSP = get_SP();
  return (400 - int(4*isSP));
}

void handleData() {
  Serial.println();
  Serial.print("www data.html served");
  //LED_toggle();
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  char uptimec[100];
  snprintf(uptimec,100,"%02d:%02d:%02d",hr, min % 60, sec % 60);

  //strncpy(Tnows,nows(),45 ); // get time to Tnows
  nows(); // and use global Tnows
  //strncpy(A_Reads,get_Ains(),100 ); // get Ains
  char PID_Reads[90]; // ________________________________________________  with data from pico_w_io.ino
  char getMODEs[5];
  if ( get_MODE() == 0 ) { snprintf(getMODEs,5,"LOCK");}
  if ( get_MODE() == 1 ) { snprintf(getMODEs,5,"MAN");} 
  if ( get_MODE() == 2 ) { snprintf(getMODEs,5,"AUTO");}
  if ( get_MODE() == 3 ) { snprintf(getMODEs,5,"CASC");}

  snprintf(PID_Reads,90," ___ Job15sec: PID: PV: %.2f SP: %.2f OUT: %.2f  [pct], MODE: %s ", get_PV(), get_SP(), get_OUT(), getMODEs );
  Serial.printf(" len PID_Reads : %d \n" ,strlen(PID_Reads));
  Serial.println(PID_Reads);

  char get_pid_details[50];
  snprintf(get_pid_details,50,"Kp=%.2f, Ki=%.2f, Kd=%.2f",get_Kp(),get_Ki(),get_Kd());
  String SVG_HTMLs = "<svg width=\"";
  SVG_HTMLs += String(SVG_W);
  SVG_HTMLs += "\" height=\"";
  SVG_HTMLs += String(SVG_H);
  SVG_HTMLs += "\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">\
<desc>show PID faceplate </desc>\
<rect x=\"1\" y=\"1\" width=\"";
  SVG_HTMLs += String(SVG_W);
  SVG_HTMLs += "\" height=\"";
  SVG_HTMLs += String(SVG_H);
  SVG_HTMLs += "\"  stroke=\"#000088\" stroke-width=\"1\" />\
<rect x=\"2\" y=\"2\" width=\"";
  SVG_HTMLs += String(SVG_4W);
  SVG_HTMLs += "\" height=\"";
  SVG_HTMLs += String(SVG_4H);
  SVG_HTMLs += "\"  stroke=\"#0000aa\" stroke-width=\"1\" />\
<rect x=\"3\" y=\"3\" width=\"";
  SVG_HTMLs += String(SVG_6W);
  SVG_HTMLs += "\" height=\"";
  SVG_HTMLs += String(SVG_6H);
  SVG_HTMLs += "\"  fill=\"#FFF8DC\" stroke=\"#0000ff\" stroke-width=\"1\" />\
<g id=\"faceplate\" style=\"visibility:visible; stroke-width:2; stroke=\"#000000\"; font-family=\"Verdana\"; font-size=\"16\";\" >\
<rect x=\"50\" y=\"40\" width=\"40\" stroke=\"#000000\" fill=\"#00ffff\" height=\"402\"/>\
<rect x=\"52\" y=\"42\" width=\"36\" stroke=\"#777777\" fill=\"#777777\" height=\"";
  SVG_HTMLs += String(get_OUTh());
  SVG_HTMLs += "\"/>\
<rect x=\"100\" y=\"61\" width=\"93\"  height=\"38\" stroke=\"#ffffff\" fill=\"#ffffff\" />\
<text x=\"150\" y=\"76\"  text-anchor=\"middle\">OUT: ";
  SVG_HTMLs += String(get_OUT(),1);
  SVG_HTMLs += "</text>\
<text x=\"150\" y=\"91\" text-anchor=\"middle\">[ pct ]</text>\
<rect x=\"200\" y=\"40\" width=\"40\" stroke=\"#000000\" fill=\"#ffff00\" height=\"402\"/>\
<rect x=\"202\" y=\"42\" width=\"36\" stroke=\"#777777\" fill=\"#777777\" height=\"";
  SVG_HTMLs += String(get_PVh());
  SVG_HTMLs += "\"/>\
<rect x=\"250\" y=\"61\" width=\"93\"  height=\"38\" stroke=\"#ffffff\" fill=\"#ffffff\" />\
<text x=\"300\" y=\"76\" text-anchor=\"middle\">PV: ";
  SVG_HTMLs += String(get_PV(),1);
  SVG_HTMLs += "</text>\
<text x=\"300\" y=\"91\" text-anchor=\"middle\">[ pct ]</text>\
<rect x=\"350\" y=\"40\" width=\"40\" stroke=\"#000000\" fill=\"#ffffff\" height=\"402\"/>\
<rect x=\"352\" y=\"42\" width=\"36\" stroke=\"#777777\" fill=\"#777777\" height=\"";
  SVG_HTMLs += String(get_SPh());
  SVG_HTMLs += "\"/>\
<rect x=\"400\" y=\"61\" width=\"93\"  height=\"38\" stroke=\"#ffffff\" fill=\"#ffffff\" />\
<text x=\"450\" y=\"76\" text-anchor=\"middle\">SP: ";
  SVG_HTMLs += String(get_SP(),1);
  SVG_HTMLs += "</text>\
<text x=\"450\" y=\"91\" text-anchor=\"middle\">[ pct ]</text>\
<rect x=\"500\" y=\"161\" width=\"93\"  height=\"38\" stroke=\"#ffffff\" fill=\"#ffffff\" />\
<text x=\"550\" y=\"176\" text-anchor=\"middle\">MODE:</text>\
<text x=\"550\" y=\"191\" font-size=\"18\" text-anchor=\"middle\">";
  SVG_HTMLs += String(getMODEs);
  SVG_HTMLs += "</text>\
</g>\
</svg>\n\
"; // make a extra string variable for SVG only and can disable the include easy

DYN_HTMLs  = "<!DOCTYPE html><html><head>\
<meta http-equiv='refresh' content='15'>\
<title>KLL engineering</title>\
<style>\
body {font-family: \"Times New Roman\", Times, serif; background-color: lightgreen;\
display:inline-block; margin: 0px auto; text-align: center;}\
h1 {color: deeppink; word-wrap: break-word; padding: 1vh; font-size: 30px;}\
p {font-size: 1.5rem; word-wrap: break-word;}\
p.dotted {margin: auto; width: 75%; font-size: 25px; text-align: center;}\
form {font-size: 2rem; }\
input[type=number] {font-size: 2rem;}\
table { width: 100%; }\
</style>\
</head>\
<body>\
<H1>PICO W</H1>\
<hr><H2>Analog IO:</H2>\
<H3> ";
  DYN_HTMLs  += String(A_Reads);
  DYN_HTMLs  += "</H3<hr><H2> PID </H2><H3>";
  DYN_HTMLs  += String(PID_Reads);
  DYN_HTMLs  += "</H3><H3>";
  DYN_HTMLs  += String(get_pid_details);
  DYN_HTMLs  += "</H3><hr><table align=\"center\" style=""width:100%""><tr><td width=\"10%\"></td><td>";
  DYN_HTMLs  += SVG_HTMLs; // _______________________________________ add SVG here
  DYN_HTMLs  += "</td>\
<td width=\"10%\"></td>\
</tr>\
</table>\
<table style=\"width:100%\"><tr><td>\
<form action=\"/data/form/OUT\" method=\"post\">\
OUT: <input type=number value=";
  DYN_HTMLs  += String(get_OUT(),1);
  DYN_HTMLs  += " step=0.1 id=\"OUT\" size=\"8\" name=\"OUT\" min=\"0\" max=\"100\" />\
</form>\
</td>\
<td>\
<form action=\"/data/form/SP\" method=\"post\">\
SP: <input type=number value=";
  DYN_HTMLs  += String(get_SP(),1);
  DYN_HTMLs  += " step=0.1 id=\"SP\" name=\"SP\" size=\"8\" min=\"0\" max=\"100\" />\
</form>\
</td></tr><tr>\
<td><p>and go MANUAL MODE</p></td>\
<td><p>and go AUTO MODE</p></td></tr></table>\
<hr>\
<H2> last MQTT to broker:</H2>\
<H3>";
  DYN_HTMLs  += MQTTs;
  DYN_HTMLs  += "</H3>\
<hr>\
<table><tr><th>\
<p><a href='http://kll.byethost7.com/kllfusion01/infusions/articles/articles.php?article_id=230'\
target='_blank' ><b>kll engineering blog</b></a></p>\
</th><th><p>rev: ";
  DYN_HTMLs  += String(rev);
  DYN_HTMLs  += "</p></th></tr><tr><td><p>";
  DYN_HTMLs  += Tnows; // from nows() in common
  DYN_HTMLs  += "</p></td><td><p>Uptime: ";
  DYN_HTMLs  += String(uptimec);
  DYN_HTMLs  += "</p></td></tr></table>\
<hr></body></html>";

  DYN_HTMLs.toCharArray(DYN_HTML,5000); // for HTML back to CHAR, still hope the creation of the string is more save as snprintf ?( about PANIC and Stack smashing protect failure!)
  //Serial.println(DYN_HTML);
  Serial.printf(" len DATA : %d \n",strlen(DYN_HTML));
  server.send(200, "text/html", DYN_HTML);
  DYN_HTML[0]= '\0'; // kind of clear
  //LED_toggle();
}

void handleSP() { // ________________________________________________ SP changed in webpage /data
  if ( server.method() == HTTP_POST ) {
    String msg = "Data SP: #";
    msg += server.args(); // 1
    msg += " , ";
    msg += server.argName(0) ;
    msg += " : ";
    msg += server.arg(0);
    Serial.println(msg);
  }
  server.sendHeader("Location", "/data",true); // REDIRECT  
  server.send(302, "text/plain", "");
  float newSP = server.arg(0).toFloat();
  set_SP(newSP); // in pico_w_io
  set_MODE(2); // in pico_w_io
}

void handleOUT() { // _______________________________________________ OUT changed in webpage /data
  if ( server.method() == HTTP_POST ) {
    String msg = "Data OUT: #";
    msg += server.args(); // 1
    msg += " , ";
    msg += server.argName(0) ;
    msg += " : ";
    msg += server.arg(0);
    Serial.println(msg);
  server.sendHeader("Location", "/data",true); // REDIRECT  
  server.send(302, "text/plain", "");
  float newOUT = server.arg(0).toFloat();
  set_OUT(newOUT); // in pico_w_io
  set_MODE(1); // in pico_w__io
  }
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

/*
void drawGraph() {
  String out;
  out.reserve(2600);
  char temp[70];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}
*/

void setup_WIFI() {
  // _______________________________________ connect to router
  WiFi.mode(WIFI_STA);
  WiFi.config(thisip); //, gateway, subnet, primaryDNS, secondaryDNS);
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

  //if (MDNS.begin("picw")) {
  //  Serial.println("MDNS responder started");
  //}

  server.on("/", handleRoot);
  //server.on("/test.svg", drawGraph);
  server.on("/data", handleData);
  server.on("/data/form/OUT", handleOUT);
  server.on("/data/form/SP", handleSP);
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
  //MDNS.update();
}