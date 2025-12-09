#include "GateControl.h"
#include "Actuator.h"
#include "CurrentProtection.h"
#include <Arduino.h>


enum GateCommand {
    CMD_NONE,
    CMD_OPEN,
    CMD_CLOSE
};

GateCommand pendingCmd = CMD_NONE;

void requestGateOpen() {
    pendingCmd = CMD_OPEN;
}

void requestGateClose() {
    pendingCmd = CMD_CLOSE;
}

void updateGateControl() {

    if (isCurrentTripA() || isCurrentTripB()) {
        //stopGate();
        pendingCmd = CMD_NONE;
        Serial.println("⚠️ Zabezpieczenie prądowe – STOP");
        return;
    }

    switch(pendingCmd) {

        case CMD_OPEN:
            openGate();
            pendingCmd = CMD_NONE;
            break;

        case CMD_CLOSE:
            closeGate();
            pendingCmd = CMD_NONE;
            break;

        default:
            break;
    }
}
