// Mks.cpp

#include "Mks.h"
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "Config.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "MksCrons.h"



bool updateCronIdByUUID(const char* targetUUID, size_t numCrons, IOTcrons newCrons[],String version) {
    for (int i = 0; i < stringArrayCount; i++) {
        if (strcmp(stringArray[i].uuid, targetUUID) == 0 ) {
          

            // stringArray[i].version = "";
            stringArray[i].version = version;
            
            // Update the number of crons
            stringArray[i].numCrons = numCrons;

            // Allocate memory for the new crons array
            stringArray[i].crons = new IOTcrons[numCrons];

            // Copy each cron from the newCrons array to the sensor's crons array
            for (size_t j = 0; j < numCrons; j++) {
                stringArray[i].crons[j] = newCrons[j];
            }     

            return true;
        }
    }
    return false; // Sensor not found
}





void mqtt_callback(char* topic, byte* message, unsigned int length) {
  try {
    
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
   
  Serial.println("") ;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }

  

  DynamicJsonDocument jsonDoc(256); // Adjust the capacity based on your JSON structure
  DeserializationError error = deserializeJson(jsonDoc, messageTemp);

  // Check for parsing errors
  if (error) {
    Serial.print(F("Error decoding JSON: "));
    Serial.println(error.c_str());
    return;
  }


  
              
  
  if(jsonDoc.containsKey("state")){    
    mksDevice device;
    device.state = jsonDoc["state"].as<const char*>();    
    device.uuid = topic;    
    triggerAction(&device);    
  }else if(jsonDoc.containsKey("configuration")){
      int cronsCount = jsonDoc["configuration"]["crons"].size();
      JsonArray arr = jsonDoc["configuration"]["crons"].as<JsonArray>();
      IOTcrons mksCrons[cronsCount];
      resetSensorCron(topic);
      int i = 0;     
      for (JsonObject cronExpr : arr) {
          const char* cron = cronExpr["cron"];   
          char cronCopy[strlen(cron) + 1];
          strcpy(cronCopy, cron);

          const char* state = cronExpr["state"];   
          char stateCopy[strlen(state) + 1];
          strcpy(stateCopy, state);

          int id = mks.setCron(cronCopy);   
          if(id != dtINVALID_ALARM_ID){
            mksCrons[i].cronId = id;
            mksCrons[i].state = strdup(stateCopy);
            Serial.println("CronId");             
           Serial.println(mksCrons[i].cronId);    
          }else{
            Serial.println("Invalid cron expression");
            char message[100];  
            sprintf(message, "Invalid cron is setting to %s", state);


            mksLog log = {topic, "FAIL",message};
            triggerLog(&log);
          }
          

          
           i++;
      }  
      if(cronsCount > 0){        
        bool updated = updateCronIdByUUID(topic, cronsCount, mksCrons, jsonDoc["version"].as<String>());        

        mksNextCronTime();
        if (!updated) {
            Serial.println("Sensor not found for the given UUID.");
        }
      }      
   }else if(jsonDoc.containsKey("debug")){
    Serial.print("Debuggin portion");
   } else{
    mksLog log = {topic, "","Unknown request recivied"};
    triggerLog(&log);      
   }
   
  }catch (const char* e) {
     
  }
  
}
