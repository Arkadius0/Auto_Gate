#include "PCA.h"
#include <Adafruit_PWMServoDriver.h>
#include "Config.h"

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(PCA_ADDR);

void initPCA() {
    pca.begin();
    pca.setPWMFreq(1000); 
        Wire.beginTransmission(0x42);
    if (Wire.endTransmission() == 0) {
        Serial.println("✔ PCA9685 wykryty na I2C!");
    } else {
        Serial.println("❌ BŁĄD: PCA9685 nie został znaleziony na I2C!");
        return;
    }
}

