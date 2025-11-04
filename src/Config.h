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
//static const int RELAY_PIN = 5;
//static const int LED_PIN = 2;
// static const int Radio_PIN = 35; // port radio 
//---actuator limit switch---
constexpr int SWA_start = 2; //  actuator A fully extended
constexpr int SWA_end = 16;  // actuator A fully retracted
constexpr int SWB_start = 18; //  actuator B fully extended
constexpr int SWB_end = 17; // actuator B fully retracted

//---H-Bridge BTS7960---
// bridge*(A)
constexpr int RPWM_A = 25;
constexpr int LPWM_A = 26;
constexpr int R_EN_A = 27;
constexpr int L_EN_A = 14;
// bridge*(B)
constexpr int RPWM_B = 22;
constexpr int LPWM_B = 23;
constexpr int R_EN_B = 21; 
constexpr int L_EN_B = 32;

//---IN226 (I2C) - adress--- 
constexpr uint8_t INA_A_ADDR = 0x40;
constexpr uint8_t INA_B_ADDR = 0x41;

//---web---
constexpr int HTTP_PORT= 80;