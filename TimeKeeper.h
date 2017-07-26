#include <TimeLib.h>
#define DEFAULT_SYNC_INTERVAL 5*60 // seconds
#define DEFAULT_SYNC_RETRY_INTERVAL 5  // seconds
const char TIME_FMT[] PROGMEM = "%02d.%02d.%04d %02d:%02d:%02d";

class TimeKeeper {
    uint32_t sysTime = 0;
    uint32_t prevMillis = 0;
    uint32_t lastSyncTime = 0;
    uint32_t lastSyncRetryTime = 0;
    uint32_t syncInterval;
    uint32_t syncRetryInterval;
  public:
    timeStatus_t status = timeNotSet;
    TimeKeeper();
    TimeKeeper(uint32_t syncInterval, uint32_t syncRetryInterval);
    time_t get();
    void set(time_t t);
    bool retrySync();
    String getStatusString();
};

TimeKeeper::TimeKeeper(uint32_t syncInterval, uint32_t syncRetryInterval):
  syncInterval(syncInterval), syncRetryInterval(syncRetryInterval) {};

TimeKeeper::TimeKeeper(): TimeKeeper(DEFAULT_SYNC_INTERVAL, DEFAULT_SYNC_RETRY_INTERVAL) {};

time_t TimeKeeper::get() {
  while (millis() - this->prevMillis >= 1000) {
    this->sysTime++;
    this->prevMillis += 1000;
  }
  return (time_t)this->sysTime;
}

void TimeKeeper::set(time_t t) {
  if (t) {
    this->sysTime = (uint32_t)t;
    this->prevMillis = millis();
    this->lastSyncTime = (uint32_t)t;
    this->lastSyncRetryTime = (uint32_t)t;
    this->status = timeSet;
  }
}

bool TimeKeeper::retrySync() {
  this->get();
  if (this->status != timeSet) {
    if (sysTime >= lastSyncRetryTime + syncRetryInterval) {
      lastSyncRetryTime = sysTime;
      return true;
    }
  } else {
    if (sysTime >= lastSyncTime + syncInterval) {
      this->status = timeNeedsSync;
      lastSyncRetryTime = sysTime;
      return true;
    }
  }
  return false;
}

String TimeKeeper::getStatusString(){
  switch (this->status){
    case timeNotSet: return PSTR("not set");
    case timeNeedsSync: return PSTR("needs sync");
    case timeSet: return PSTR("synced");
  }
  return "";
}

void getFormatedTime(char* bf, size_t bfsz, time_t time0) {
  snprintf_P(bf, bfsz, TIME_FMT, day(time0), month(time0), year(time0), hour(time0), minute(time0), second(time0));
}

String TimeToString(time_t time0) {
  char bf[20];
  getFormatedTime(bf, 20, time0);
  return String(bf);
}
