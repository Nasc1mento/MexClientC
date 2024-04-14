#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "AppConfig.h"
#include "Mex.h"

int random_id = random(0, 255);
Mex mex(BROKER_HOST, BROKER_PORT, random_id);
int comunication_done = -1;


void wifi_connect() {
  Serial.print("Connecting to ");
  WiFi.begin(SSID, PASSWORD);

  Serial.println("Connecting to Wifi");
  while (WiFi.status() != WL_CONNECTED) {   
    delay(500);
    Serial.print(".");
    delay(500);
  }

  Serial.println("Connected to Wifi");
}

void mex_connect() {
  int status;
  do {
    status = mex.connect();
    delay(500);
    Serial.print(".");
    delay(500);
  } while (status != MEX_CONNECTED);

  Serial.println("Connected to MEX");
}



void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  while(!Serial) { }

  // Serial.flush();
  wifi_connect();
  mex_connect();

 while (comunication_done == -1) {
    comunication_done = mex.publish("topic", "message");
    delay(500);
  }

  if (comunication_done == 0) {
    ESP.deepSleep(5000);
  }
}

void loop() {
  
  mex.publish("topic", "message");
  delay(5000);
  
}
