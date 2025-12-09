#include <Arduino.h>
#include "Actuator.h"
#include "Config.h"
#include "PCA.h"
#include <Adafruit_PWMServoDriver.h>

extern Adafruit_PWMServoDriver pca;
ActuatorState actuatorAState = STOP;
ActuatorState actuatorBState = STOP;

void actuatorAForward() {
  pca.setPWM(RPWM_A, 0,4095);
  pca.setPWM(LPWM_A, 0,0);
  Serial.printf("A Forward: RPWM_A=%d LPWM_A=%d\n", 4095, 0);
  actuatorAState = MOVING_FORWARD;
}

void actuatorABackward() {
  pca.setPWM(RPWM_A, 0,0);
  pca.setPWM(LPWM_A, 0,4095);
  actuatorAState = MOVING_BACKWARD;
}

void actuatorAStop() {
   pca.setPWM(RPWM_A, 0,0);
   pca.setPWM(LPWM_A, 0,0);
   pca.setPWM(R_EN_A, 0,0);
   pca.setPWM(L_EN_A, 0,0);
  actuatorAState = STOP;
    Serial.println("STOP-A");
}

void actuatorBForward() {
   pca.setPWM(RPWM_B, 0,4095);
   pca.setPWM(LPWM_B, 0,0);
  actuatorBState = MOVING_FORWARD;
}

void actuatorBBackward() {
   pca.setPWM(RPWM_B, 0,0);
   pca.setPWM(LPWM_B, 0,4095);
  actuatorBState = MOVING_BACKWARD;
}

void actuatorBStop() {
   pca.setPWM(RPWM_B, 0,0);
   pca.setPWM(LPWM_B, 0,0);
   pca.setPWM(R_EN_B, 0,0);
   pca.setPWM(L_EN_B, 0,0);
  actuatorBState = STOP;
  Serial.println("STOP-B");
}

void updateActuator() {
  switch (actuatorAState) {
    case MOVING_FORWARD:
      if (digitalRead(SWA_end) == HIGH) actuatorAStop();
      //Serial.print("SWA_end: "); Serial.println(digitalRead(SWA_end));
      break;
    case MOVING_BACKWARD:
      if (digitalRead(SWA_start) == HIGH) actuatorAStop();
      //Serial.print("SWA_start: "); Serial.println(digitalRead(SWA_start));
      break;
    default: break;
  }

  switch (actuatorBState) {
    case MOVING_FORWARD:
      if (digitalRead(SWB_start) == HIGH) actuatorBStop();
      //Serial.print("SWB_end: "); Serial.println(digitalRead(SWA_end));
      break;
    case MOVING_BACKWARD:
      if (digitalRead(SWB_end) == HIGH) actuatorBStop();
      //Serial.print("SWB_start: "); Serial.println(digitalRead(SWA_start));
      break;
    default: break;
  }
}

void openGate() {
   pca.setPWM(R_EN_A, 0,4095);
   pca.setPWM(L_EN_A, 0,4095);
   pca.setPWM(R_EN_B, 0,4095);
   pca.setPWM(L_EN_B, 0,4095);
   actuatorAForward();
   actuatorBForward();
   Serial.println("Opening gate...");
}

void closeGate() {
   pca.setPWM(R_EN_A, 0,4095);
   pca.setPWM(L_EN_A, 0,4095);
   pca.setPWM(R_EN_B, 0,4095);
   pca.setPWM(L_EN_B, 0,4095);
   actuatorABackward();
   actuatorBBackward();
   Serial.println("Closing gate...");
}

void stopGate(){
    actuatorAStop();
    actuatorBStop();
}