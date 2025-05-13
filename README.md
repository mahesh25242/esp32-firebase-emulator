# Overview
The zip file in the root folder contains the library used for cron. This library was created from CronAlarms. However, the ccronexpr it uses is outdated, 
so I replaced it with a new one developed at [Supertinycron](https://github.com/exander77/supertinycron). To import this into the Arduino IDE, navigate to the "Sketch" menu, then hover over "Include Library". F
rom there, click "Add .ZIP Library" and select MksCrons.zip from our root folder.


## Additionally, we are using the following libraries:
```
NTPClient
PubSubClient
ArduinoJson
```
Setup in the main.ino file, you'll find the following configuration:
```cpp
char* ssid = "BSNL-FTTH_EXT"; // WiFi name
char* password = ""; // WiFi password
char* mqtt_server = "e6e9ee8b38ee449b9d81904534040e41.s2.eu.hivemq.cloud"; // HiveMQ URL
char *mqttUsername = "mks123";  // Replace with your MQTT username (if needed) 
char *mqttPassword = "mks123"; // HiveMQ password
```

Below this configuration, you'll see:


```cpp
sensors stringArray[] = {
  {"MKS/TH/001",  "0", defaultCrons, 1},
  {"MKS/ACT/001", "0",  defaultCrons, 1},
  {"MKS/CO2/001", "0",  defaultCrons, 1},
  {"MKS/PCT/001",  "0", defaultCrons, 1}
};
```
Here, we are adding sensor information. "MKS/TH/001", "MKS/ACT/001", "MKS/CO2/001", "MKS/PCT/001", etc. are the sensor UUIDs. You can change these with the UUIDs of each sensor. or can add new sensor with same structure. DONT CHNAGE ANY OTHERS LIKE defaultCrons or 1 it should keep as it is

## Functionality
In the same file, you'll find a function called triggerAction. This function is called whenever an action is required. For example, FMS will trigger and turn on for "MKS/ACT/001", causing this function to trigger. Also, if a cron is triggered, it will be executed with this function. Currently, this function acts as an IoT emulator. Once triggered, it will publish random data for humidity, temperature, and CO2. If it's a picture, a base64 string is statically set.

Additionally, in the main.ino file:

```cpp
unsigned long previousMillis = 0;  // Variable to store the last time the function was called
const long interval = 600000;       // Interval in milliseconds (10 minutes)
```

This is used to check the version every 10 minutes. You can change this if you want a different interval.

in config.h you can find 
```cpp
static const char *root_ca PROGMEM 
```
its the ca certificate for secure connection with mqtt


currently this will work in manuel action from fms like turn on, off, clik on temprature button, humidity button etc. and also in cron too. in case of cron i am not sure all cronn  expression is works
```diff
- conditional turn on will not work 
```

Here we are setting the time zone in cet but it wont work the daylight saving.  
