#define MQTT_MAX_PACKET_SIZE 6000
#include <PubSubClient.h>
#include <Wire.h>
#include <WiFiClientSecure.h>
#include "Enkitek.h"
#include "Config.h"
#include <ArduinoJson.h>

IOTcrons defaultCrons[] = {{-1, ""}};

char* ssid = "BSNL-FTTH_EXT";
char* password = "punnathanath1234";
char* mqtt_server = "e6e9ee8b38ee449b9d81904534040e41.s2.eu.hivemq.cloud";
char *mqttUsername = "enkitek";  // Replace with your MQTT username (if needed)
char *mqttPassword = "EnkiDev123456";  // Replace with your MQTT password (if needed)

sensor stringArray[] = {
  {"MKS/TH/001",  "0", defaultCrons, 1},
  {"MKS/ACT/001",  "0", defaultCrons, 1},
  {"MKS/CO2/001",  "0", defaultCrons, 1},
  {"MKS/PCT/001",  "0", defaultCrons, 1}
};

unsigned long previousMillis = 0;  // Variable to store the last time the function was called
const long interval = 600000;//600000;       // Interval in milliseconds (10 minutes)

int stringArrayCount = sizeof(stringArray) / sizeof(stringArray[0]);

// WiFiClient espClient;
WiFiClientSecure espClient;

PubSubClient client(espClient);
Enkitek enki;
// LED Pin
const int ledPin = 4;

void triggerAction(enkiDevice* device){
  DynamicJsonDocument jsonDoc(256);
  jsonDoc["sensorUUID"] = device->uuid;
  char* topic = "sensor-actuator";
  const char* state = device->state;

  if(strcmp(state, "on") == 0){      
      jsonDoc["state"] = "on";            
    } else if(strcmp(state, "off") == 0){       
        jsonDoc["state"] = "off";        
    } else if(strcmp(state, "temperature") == 0){                
        jsonDoc["temperature"] = String(random(20, 68));;        
    } else if(strcmp(state, "humidity") == 0){                
        jsonDoc["humidity"] = String(random(30, 70));        
    } else if(strcmp(state, "co2") == 0){              
        jsonDoc["co2"] = String(random(23, 30));        
    } else if(strcmp(state, "picture") == 0){              
        jsonDoc["image"] = "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mNk+A8AAQUBAScY42YAAAAASUVORK5CYII=";        
    }else if(strcmp(state, "healthCheck") == 0){   
        topic = "sensor-logging";
        jsonDoc["healthCheck"] = "OK";
        jsonDoc["comments"] = "Device check completed";             
    }
  String jsonString;
  serializeJson(jsonDoc, jsonString);    
  enkiMqttPblish(topic, jsonString);    
}

void triggerLog(enkiLog* log){
 DynamicJsonDocument jsonDoc(256);
  jsonDoc["sensorUUID"] = log->uuid;
  if (log->healthCheck != nullptr && log->healthCheck[0] != '\0') {
    jsonDoc["healthCheck"] = log->healthCheck;
  }
  
  jsonDoc["comments"] = log->comments;     
  String jsonString;
  serializeJson(jsonDoc, jsonString);    
  enkiMqttPblish("sensor-logging", jsonString);    
}
void enkiMqttPblish(char* topic, String message) {
  const char* payload = message.c_str();
  client.publish(topic, payload);   
  Serial.println("") ;
  Serial.print(topic);
  Serial.print(" Message published successfully ");  
  Serial.println("") ;
  Serial.print(message);
}

void versionCheck(){
  DynamicJsonDocument jsonDoc(256);
  String jsonString;      

   for (int i = 0; i < stringArrayCount; i++) {
    jsonDoc["sensorUUID"] =  stringArray[i].uuid;
    jsonDoc["version"] =  stringArray[i].version ;
  
    serializeJson(jsonDoc, jsonString);
    enkiMqttPblish("sensor-version",jsonString);   
  }
}

void reconnect() {
  // Loop until we're reconnected
  if(WiFi.status() != WL_CONNECTED){
    // wifiConnected = false;
    enki.setup_wifi();    
  }
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect    
    if (client.connect("mkstest", mqttUsername, mqttPassword)) { //mqttUsername, mqttPassword
      Serial.println("connected");
      // Subscribe   
      for (int i = 0; i < stringArrayCount; i++) {
        client.subscribe(stringArray[i].uuid);    
      }      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");      
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));  
  enki.setup_wifi();    
  

  espClient.setCACert(root_ca);
  client.setServer(mqtt_server, 8883);
  client.setCallback(mqtt_callback);
  boolean res = client.setBufferSize(50*1024);
  reconnect();  
  enki.setCronInit();
  
  versionCheck();
  pinMode(ledPin, OUTPUT);  
}





void loop() {
  if (!client.connected()) {
    reconnect();
  }
  

  client.loop();
  enki.setCronDelay();  
 
  unsigned long currentMillis = millis();  // Get the current time in milliseconds

    // Check if it's time to trigger the function
    if (currentMillis - previousMillis >= interval) {
      // Save the last time the function was called
      previousMillis = currentMillis;

      // Call your function here
      versionCheck();
    }
}


  