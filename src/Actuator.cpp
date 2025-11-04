#include <Actuator.h>
#include <Config.h>

ActuatorState actuatorAState = STOP;
ActuatorState actuatorBState = STOP;

void actuatorAForward() {
  digitalWrite(RPWM_A, HIGH);
  digitalWrite(LPWM_A, LOW);
  actuatorAState = MOVING_FORWARD;
}

void actuatorABackward() {
  digitalWrite(RPWM_A, LOW);
  digitalWrite(LPWM_A, HIGH);
  actuatorAState = MOVING_BACKWARD;
}

void actuatorAStop() {
  digitalWrite(RPWM_A, LOW);
  digitalWrite(LPWM_A, LOW);
  digitalWrite(R_EN_A, LOW);
  digitalWrite(L_EN_A, LOW);
  actuatorAState = STOP;
    Serial.println("STOP");
}

void actuatorBForward() {
  digitalWrite(RPWM_B, HIGH);
  digitalWrite(LPWM_B, LOW);
  actuatorBState = MOVING_FORWARD;
}

void actuatorBBackward() {
  digitalWrite(RPWM_B, LOW);
  digitalWrite(LPWM_B, HIGH);
  actuatorBState = MOVING_BACKWARD;
}

void actuatorBStop() {
  digitalWrite(RPWM_B, LOW);
  digitalWrite(LPWM_B, LOW);
  digitalWrite(R_EN_B, LOW);
  digitalWrite(L_EN_B, LOW);
  actuatorBState = STOP;
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
  digitalWrite(R_EN_A, HIGH);
  digitalWrite(L_EN_A, HIGH);
  digitalWrite(R_EN_B, HIGH);
  digitalWrite(L_EN_B, HIGH);
  actuatorAForward();
  actuatorBForward();
  Serial.println("Opening gate...");
}

void closeGate() {
  digitalWrite(R_EN_A, HIGH);
  digitalWrite(L_EN_A, HIGH);
  digitalWrite(R_EN_B, HIGH);
  digitalWrite(L_EN_B, HIGH);
  actuatorABackward();
  actuatorBBackward();
  Serial.println("Closing gate...");
}
