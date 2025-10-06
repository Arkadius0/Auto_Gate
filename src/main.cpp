#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Config.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <RCSwitch.h>


// --- HTTPserver -----
WebServer server(HTTP_PORT);

// --- WiFi client + MQTT ---
WiFiClientSecure espClient;   // Secure, bo HiveMQ w chmurze używa TLS
PubSubClient mqttClient(espClient);

//radio state
bool lastRadioState = HIGH;

// --- gate control functiions ---
void openGate(){
  Serial.println("Gate: OPEN command");
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}

void closeGate(){
  Serial.println("Gate: CLOSE command");
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  digitalWrite(RELAY_PIN, LOW);

}

// --- WWW ---
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width'>";
  html += "<title>Gate - test</title></head><body style='font-family:sans-serif;text-align:center;padding:30px;'>";
  html += "<h2>Gate Controller - test</h2>";
  html += "<p><a href='/open'><button style='font-size:20px;padding:12px 24px;'>Otwórz (test)</button></a></p>";
  html += "<p><small>IP: " + WiFi.localIP().toString() + "</small></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}
void handleOpen(){
  openGate();
  server.send(200, "text/plain", "Gate opened");
}
void handleClose(){
  closeGate();
  server.send(200, "text/plain", "Gate closed");
}


// --- MQTT callback ---
void mqttCallback(char* topic, byte* payload, unsigned int length){
  String msg;
  for (unsigned int i = 0; i < length; i++){
    msg += (char)payload[i];
  }
  Serial.print("MQTT msg on[");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  if(msg == "OPEN"){openGate();}
  else if(msg == "CLOSE"){closeGate();}
}

// --- WiFi ---
void connectWiFi(){
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("CONNECTED");
  Serial.print("IP:  ");
  Serial.print(WiFi.localIP());
}

// --- mqtt ---
void connectMQTT(){
 espClient.setInsecure();
 mqttClient.setServer(MQTT_HOST, MQTT_PORT);
 mqttClient.setCallback(mqttCallback);

 while (!mqttClient.connected())
 {
  Serial.print("Connecting to MQTT . . .");
  if(mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)){
    Serial.print("connected!");
    mqttClient.subscribe("gate/cmd");
  }else {
    Serial.print("failed, rc=");
    Serial.println(mqttClient.state());
    delay(2000);
  }
 }
}


void setup(){
  Serial.begin(115200);
  delay(100);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  pinMode(Radio_PIN, INPUT_PULLUP);
  lastRadioState = digitalRead(Radio_PIN);
 
  connectWiFi();
  espClient.setInsecure();
  connectMQTT();

  server.on("/", handleRoot);
  server.on("/open", handleOpen);
  server.on("/close", handleClose);
  server.begin();

}

void loop(){
  server.handleClient();
  if(!mqttClient.connected()){
    connectMQTT();
  }
  mqttClient.loop();

  bool currentState = digitalRead(Radio_PIN);
  if(lastRadioState == HIGH && currentState == LOW){
    Serial.println("Pilot: ON detected -> OPEN GATE");
    openGate();
  }
  if(lastRadioState == LOW && currentState == HIGH){
    Serial.println("Pilot: OFF detected -> CLOSE GATE");
    closeGate();
  }
  lastRadioState = currentState;
}

