#pragma one

#include "Config.h"
#include <Arduino.h>

typedef enum {
  GATE_STOPPED,
  GATE_OPENING,
  GATE_CLOSING,
  GATE_OPEN,
  GATE_CLOSED,
  GATE_ERROR
} GateState_t;

void gateControlInit();
void gateControlLoop();

void requestOpen();
void requestClose();
void requestToggle();

GateState_t getGateState();
bool isGateBusy();