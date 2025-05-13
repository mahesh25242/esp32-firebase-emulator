// Enkitek.cpp

#include "Enkitek.h"
#include <Arduino.h>
#include "EnkitekCrons.h"
#include <time.h>    
#include "Config.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#define MAX_RETRIES 5


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

bool updateTime() {
    for (int i = 0; i < MAX_RETRIES; i++) {
        if (timeClient.update()) {
            return true; // Time update successful
        }
        delay(1000); // Wait before retrying
    }
    return false; // Exceeded max retries, time update failed
}


void resetSensorCron(const char* targetUUID) {
    for (int i = 0; i < stringArrayCount; i++) {
        if (strcmp(stringArray[i].uuid, targetUUID) == 0 ) {
            // Update the number of crons
            int numCrons = stringArray[i].numCrons;   
            
            for (int j = 0; j < numCrons; j++) {
              Cron.free(stringArray[i].crons[j].cronId); 
              stringArray[i].crons[j].cronId;             
            }  
            IOTcrons enkiCrons[1];         
            enkiCrons[0].cronId = dtINVALID_ALARM_ID;
            enkiCrons[0].state = "";
            stringArray[i].crons[0] = enkiCrons[0];            
            stringArray[i].numCrons = 0;
          
        }
    }
}


struct sensor* findSensorByCronId(const int cronId) {
    for (int i = 0; i < stringArrayCount; i++) {
        int numCrons = stringArray[i].numCrons;
        for (int j = 0; j < numCrons; j++) {
            if (stringArray[i].crons[j].cronId == cronId) {
              enkiDevice device = {
                stringArray[i].uuid,                
                stringArray[i].crons[j].state
              };
              triggerAction(&device);
              return &stringArray[i]; // Found the sensor, return a pointer to it
            }
        }
    }
    return NULL; // Sensor not found
}

void enkiNextCronTime(){
  time_t nextTriggerAny = Cron.getNextTrigger();
  Serial.print("Next scheduled alarm for any: ");
  Serial.println(ctime(&nextTriggerAny));

}
void enkiNextCronTime(const int cronId){
  time_t nextTriggerAny = Cron.getNextTrigger(cronId);


  Serial.print("Next scheduled alarm for any: ");
  Serial.println(ctime(&nextTriggerAny));

}
void CronCount(){
  Serial.println(Cron.count());  
}
void Repeats() {   
  sensor* foundSensor =  findSensorByCronId(Cron.getTriggeredCronId());
  if (foundSensor != NULL) {
    
    Serial.println("found");
    Serial.println(foundSensor->uuid);
  }

}

void setTimeFaild(){
  enkiLog log;
  for (int i = 0; i < stringArrayCount; i++) {      
      log = {stringArray[i].uuid, "","Failed to set system time"};
      triggerLog(&log);      
    }  
}


void Enkitek::setCronDelay(){
  Cron.delay();
}
void Enkitek::setCronInit() {  
  timeClient.setTimeOffset(3600); 
  if (!updateTime()) {
    Serial.println("Failed to obtain time");    
    setTimeFaild();
    return;
  }
  time_t epochTime = timeClient.getEpochTime();
  timeval tv;
  tv.tv_sec = epochTime;
  tv.tv_usec = 0;
  if (settimeofday(&tv, nullptr) != 0) {    
    Serial.println("Failed to set system time"); 
    Serial.print(epochTime) ;
    setTimeFaild();       
    // esp_restart();
    return;
  }
  Serial.println(timeClient.getFormattedTime());

// Serial.println("Cron setting");
//     struct tm tm_newtime; // set time to Saturday 8:29:00am Jan 1 2011
//   tm_newtime.tm_year = 2011 - 1900;
//   tm_newtime.tm_mon = 1 - 1;
//   tm_newtime.tm_mday = 1;
//   tm_newtime.tm_hour = 8;
//   tm_newtime.tm_min = 29;
//   tm_newtime.tm_sec = 0;
//   tm_newtime.tm_isdst = 0;

//   timeval tv = { mktime(&tm_newtime), 0 };
//   settimeofday(&tv, nullptr);

  
    // Serial.println(data);
}
int Enkitek::setCron(char* cronExp) {    
  return Cron.create(cronExp, Repeats, false);             
}
