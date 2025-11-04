#pragma once

#include <Arduino.h>

enum ActuatorState{STOP, MOVING_FORWARD, MOVING_BACKWARD};
extern ActuatorState actuatorAState;
extern ActuatorState actuatorBState;

void actuatorAForward();
void actuatorABackard();
void actuatorASTOP();

void actuatorBForward();
void actuatorBBackward();
void actuatorBSTOP();

void updateActuator();
void openGate();
void closeGate();
