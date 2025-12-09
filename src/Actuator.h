#pragma once

enum ActuatorState{STOP, MOVING_FORWARD, MOVING_BACKWARD};
extern ActuatorState actuatorAState;
extern ActuatorState actuatorBState;

void actuatorAForward();
void actuatorABackward();
void actuatorAStop();

void actuatorBForward();
void actuatorBBackward();
void actuatorBStop();

void updateActuator();

void openGate();
void closeGate();
void stopGate();