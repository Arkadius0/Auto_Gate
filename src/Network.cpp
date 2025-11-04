#include<Network.h>
#include<config.h>
#include<WiFi.h>
#include<WiFiClientSecure.h>
#include<PubSubClient.h>
#include<Actuator.h>

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

void mqttCallback(char* topic, byte*playload, unsigned int lenght){
    String msg;
    for(unsigned int i=0; i<lenght; i++) msg += (char)playload[i];
    if(msg == "OPEN") openGate();
    else if (msg == "CLOSE") closeGate();
}

void connectWiFi(){
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.print("\nWIFI connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

void connectMQTT(){
    espClient.setInsecure();
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);

    while (!mqttClient.connected()){
        if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)){
            Serial.println("MQTT connected!");
            mqttClient.subscribe("gate/cmd");
        }
        else{
            Serial.print("Failed");
            Serial.println(mqttClient.state());
            delay(2000);
        }
    }
}

void mqttloop(){
    if(!mqttClient.connected()) connectMQTT();
    mqttClient.loop();
}