#pragma once 

#include<stdint.h>

//---wifi---
static const char* WIFI_SSID = "FunBox2-D9BB";
static const char* WIFI_PASS = "D41F7D4DA5C346F67195DF9929";

//---ap---
static const char* AP_SSID = "GATE_AP";
static const char* AP_PASS = "gate1234";

//---MQTT---
static const char MQTT_HOST[] = "d2adb2545fca4e8da98f406ba3dba5fb.s1.eu.hivemq.cloud"; //URL
constexpr int MQTT_PORT = 8883; //connection port mqtt
static const char MQTT_USER[] = "Gate1"; // username mqtt
static const char MQTT_PASS[] = "Gate12345"; // password
static const char MQTT_CLIENT_ID[] = "ESP_GATE";


//---PINS---
static const int RELAY_PIN = 5;
static const int LED_PIN = 2;

//---IN226 (I2C) - adress--- 
static const uint8_t INA_A_ADDR = 0x40;
static const uint8_t INA_B_ADDR = 0x41;

//---433HZ receiver---
static const int RECV_433_PIN = 15;

//---web---
static const uint16_t HTTP_PORT= 80;