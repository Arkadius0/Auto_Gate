#include "RF433.h"
#include "Config.h"
#include "Actuator.h"
#include <RCSwitch.h>


RCSwitch rf433;

void RF433Init(){
    rf433.enableReceive(RFpin);
    Serial.print("RF433 ok");
}

void Pilot433(){
    if (rf433.available()){

        unsigned long value = rf433.getReceivedValue();
        rf433.resetAvailable();
        Serial.print("RF recived:" );
        Serial.println(value);

        if (value == P1ON || value == P2ON) {
            Serial.println("Pilot OK -> ON");
            openGate();
        } 
        else if (value == P1OFF || value == P2OFF) {
            Serial.println("Pilot OK -> OFF");
            closeGate();
        } 
        else {
            Serial.println("Pilot wrong code");
        }

    }
}