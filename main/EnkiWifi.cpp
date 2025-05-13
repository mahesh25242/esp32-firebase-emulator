// Enkitek.cpp

#include "Enkitek.h"
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "Config.h"




void Enkitek::setup_wifi() {  
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    // wifiConnected = false;
    delay(500);
    Serial.print(".");
  }
  // wifiConnected = true;
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
