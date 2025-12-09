#include "CurrentProtection.h"
#include "Config.h"
#include "Actuator.h"
#include <INA226.h>
#include <Wire.h>

INA226 INAA(INA_A_ADDR);
INA226 INAB(INA_B_ADDR);

constexpr float CURRENT_THRESHOLD_A = 3.0;
constexpr float CURRENT_THRESHOLD_B = 3.0;

unsigned long startTime = 0;

void initINA() {
    if (INAA.begin()) Serial.println("INA226 A OK");
    else Serial.println("INA226 A FAIL");
    if (INAB.begin()) Serial.println("INA226 B OK");
    else Serial.println("INA226 B FAIL");

    INAA.setMaxCurrentShunt(3.5, 0.002);
    INAB.setMaxCurrentShunt(3.5, 0.002);
}

void ProtectA_nonBlocking() {
    static unsigned long overCurrentStart = 0;
    float current = INAA.getCurrent_mA() / 1000.0;

    if (current >= CURRENT_THRESHOLD_A) {
        if (overCurrentStart == 0) overCurrentStart = millis();
        else if (millis() - overCurrentStart > 100) {
            ///actuatorAStop();
            delay(400);
            //actuatorABackward();
            overCurrentStart = 0;
        }
    } else {
        overCurrentStart = 0;
    }
}

bool isCurrentTripA() {
    return INAA.getCurrent_mA()/1000.0 >= CURRENT_THRESHOLD_A;
}

bool isCurrentTripB() {
    return INAB.getCurrent_mA()/1000.0 >= CURRENT_THRESHOLD_B;
}

void scanI2C() {
    byte error, address;
    for(address=1; address<127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if(error==0) Serial.printf("Found I2C device: 0x%X\n", address);
    }
}
