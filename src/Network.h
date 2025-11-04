#pragma once

#include<Arduino.h>
#include<Config.h>

void connectWiFi();
void connectMQTT();
void mqttloop();