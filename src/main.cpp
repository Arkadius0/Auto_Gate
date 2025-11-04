#include <Arduino.h>
#include "Config.h"
#include "Actuator.h"
#include "Network.h"
#include "WebServerModule.h"
//#include "Radio.h"

void setup() {
  Serial.begin(115200);
  connectWiFi();
  connectMQTT();
  initWebServer();
  //initRadio();
  
  pinMode(RPWM_A, OUTPUT);
  pinMode(LPWM_A, OUTPUT);
  pinMode(RPWM_B, OUTPUT);
  pinMode(LPWM_B, OUTPUT);

  pinMode(R_EN_A, OUTPUT);
  pinMode(L_EN_A, OUTPUT);
  pinMode(R_EN_B, OUTPUT);
  pinMode(L_EN_B, OUTPUT);

  pinMode(SWA_start, INPUT_PULLUP);
  pinMode(SWA_end, INPUT_PULLUP);
  pinMode(SWB_start, INPUT_PULLUP);
  pinMode(SWB_end, INPUT_PULLUP);
}

void loop() {
  mqttloop();
  webServerLoop();
  updateActuator();
  //updateRadio();
  delay(100);
}

