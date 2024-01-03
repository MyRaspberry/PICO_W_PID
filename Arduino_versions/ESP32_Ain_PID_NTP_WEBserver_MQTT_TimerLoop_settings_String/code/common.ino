// common.ino
#include <Esp.h>
#include <time.h>


#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET     0
#define UTC_OFFSET_DST 0

void setup_NTP() { // _______________________________________________ used in wifi setup
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
}


int TZs = 7*3600; // ____________________________________________________ add TZ, Time zone in Thailand (GMT+7) 


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
