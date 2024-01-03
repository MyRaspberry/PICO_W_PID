// common.ino
#include <time.h>

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

}
