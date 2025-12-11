#include <Arduino.h>
#include "Config.h"
#include "Actuator.h"
#include "Network.h"
#include "WebServerModule.h"
#include "CurrentProtection.h"
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include "PCA.h"
#include <RCSwitch.h>
#include <SPI.h>
#include "RF433.h"
#include "GateControl.h"


void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); 
  connectWiFi();
  connectMQTT();
  initWebServer();
  scanI2C();
  initINA();
  initPCA();
  RF433Init();
  gateControlInit();

  pinMode(SWA_start, INPUT_PULLUP);
  pinMode(SWA_end, INPUT_PULLUP);
  pinMode(SWB_start, INPUT_PULLUP);
  pinMode(SWB_end, INPUT_PULLUP);
}

void loop() {
  mqttloop();
  webServerLoop();
  gateControlLoop();
  ProtectA_nonBlocking();
  Pilot433(); 
  delay(100);
}