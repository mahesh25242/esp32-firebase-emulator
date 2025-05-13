#ifndef MKS_H
#define MKS_H
#include <Arduino.h>
#include <PubSubClient.h>
#include "Config.h"


class Enkitek {
  public:      
      void setup_wifi();      
      void setCronInit();   
      void setCronDelay();       
      int setCron(char* cronExp);            
};


void mqtt_callback(char* topic, byte* message, unsigned int length);    
void mksMqttPblish(char* topic, String message);
void triggerAction(enkiDevice* device);
void resetSensorCron(const char* topic);
void triggerLog(enkiLog* log);

void mksNextCronTime();
void mksNextCronTime(const int cronId);

extern Mks mks;
#endif
