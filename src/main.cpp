#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Config.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <RCSwitch.h>
#include <INA236.h>

// --- HTTPserver -----
WebServer server(HTTP_PORT);

// --- WiFi client + MQTT ---
WiFiClientSecure espClient;   // Secure, bo HiveMQ w chmurze używa TLS
PubSubClient mqttClient(espClient);

// --- radio state ---
bool lastRadioState = HIGH;

// --- INA266 ---
INA236 inaA, inaB;

enum ActuatorState {STOP, MOVING_FORWARD, MOVING_BACKWARD};
ActuatorState actuatorAState = STOP;
ActuatorState actuatorBState = STOP;

// function control actuator 
void actuatorAForward(){
  digitalWrite(RPWM_A, HIGH); digitalWrite(LPWM_A, LOW); actuatorAState = MOVING_FORWARD;
}
void actuatorABackward(){
  digitalWrite(RPWM_A, LOW); digitalWrite(LPWM_A, HIGH); actuatorAState = MOVING_BACKWARD;
}
void actuatorAStop(){
  digitalWrite(RPWM_A, LOW); digitalWrite(LPWM_A, LOW); actuatorAState = STOP;
}
void actuatorBForward(){
  digitalWrite(RPWM_B, HIGH); digitalWrite(LPWM_B, LOW); actuatorBState = MOVING_FORWARD;
}
void actuatorBBackward(){
  digitalWrite(RPWM_B, LOW); digitalWrite(LPWM_B, HIGH); actuatorBState = MOVING_BACKWARD;
}
void actuatorBStop(){
  digitalWrite(RPWM_B, LOW); digitalWrite(LPWM_B, LOW); actuatorBState = STOP;
}

// --- actualization state actuator
void updateActuator(){
  switch (actuatorAState)
  {
  case MOVING_FORWARD: if (digitalRead(SWA_end)==LOW) actuatorAStop(); break;
  case MOVING_BACKWARD: if (digitalRead(SWA_start)==LOW) actuatorAStop(); break;
  case STOP: break;
  }
  switch (actuatorBState)
  {
  case MOVING_FORWARD: if (digitalRead(SWB_end)==LOW) actuatorAStop(); break;
  case MOVING_BACKWARD: if (digitalRead(SWB_start)==LOW) actuatorAStop(); break;
  case STOP: break;
  }
}

// --- gate control functiions ---
void openGate(){
 actuatorAForward();
 actuatorBForward();
}

void closeGate(){
actuatorABackward();
actuatorBBackward();

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

  pinMode(RPWM_A, OUTPUT); pinMode(LPWM_A, OUTPUT); pinMode(R_EN_A, OUTPUT); pinMode(L_EN_A, OUTPUT);
  pinMode(SWA_end, INPUT_PULLUP); pinMode(SWA_start, INPUT_PULLUP);
  pinMode(RPWM_B, OUTPUT); pinMode(LPWM_B, OUTPUT); pinMode(R_EN_B, OUTPUT); pinMode(L_EN_B, OUTPUT);
  pinMode(SWB_end, INPUT_PULLUP); pinMode(SWB_start, INPUT_PULLUP);
  pinMode(Radio_PIN, INPUT_PULLUP);
  lastRadioState = digitalRead(Radio_PIN);

  inaA.begin();
  inaB.begin();
 
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

  updateActuator();

    float currentA = inaA.getCurrent(); float voltageA = inaA.getBusVoltage();
    float currentB = inaB.getCurrent(); float voltageB = inaB.getBusVoltage();
    Serial.print("Actuator A: "); Serial.print(voltageA); Serial.print("V "); Serial.print(currentA); Serial.println("A");
    Serial.print("Actuator B: "); Serial.print(voltageB); Serial.print("V "); Serial.print(currentB); Serial.println("A");

    delay(200);
}

