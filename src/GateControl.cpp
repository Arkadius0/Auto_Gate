#include "GateControl.h"
#include "Actuator.h"
#include "CurrentProtection.h"
#include <Arduino.h>
#include "Config.h"
#include "PCA.h"
#include <Adafruit_PWMServoDriver.h>

extern Adafruit_PWMServoDriver pca;

// --- Timeout and fuses
static const uint16_t RAMP_TARGET = 4095; // full value PWM
static const uint16_t RAMP_STEP = 120; //step by step PWM change
static const uint16_t RAMP_INTERVAL_MS = 20; //increasing

static const uint32_t OPERATION_TIMEOUT_MS = 70UL * 1000UL; // max movement time in ms 

static uint32_t errorStartTime = 0; // czas kiedy brama weszła w stan ERROR
static const uint32_t ERROR_RESET_DELAY_MS = 2500; // 2,5 sekundy

// --- Interial State ---
static GateState_t gateState = GATE_STOPPED;

typedef enum {CMD_NONE, CMD_OPEN, CMD_CLOSE} PendingCMD_t;
static PendingCMD_t pendingCMD = CMD_NONE;

// --- Ramp controller for PWM ---
struct Ramp
{   
    bool active = false;
    bool rampUp = true;
    uint16_t value = 0; 
    uint32_t lastMillis = 0;
};

static Ramp rampA; // for Actuator A 
static Ramp rampB; // for Actuator B

// --- Operation bookkeeping
static uint32_t operationStart = 0;

// helper 
static void setEnableA(uint16_t pwm){
    pca.setPWM(R_EN_A, 0, pwm);
    pca.setPWM(L_EN_A, 0, pwm);
}

static void setEnableB(uint16_t pwm){
    pca.setPWM(R_EN_B, 0, pwm);
    pca.setPWM(L_EN_B, 0, pwm);
}

// ---Request API---
void requestOpen(){
    pendingCMD = CMD_OPEN;
}
void requestClose(){
    pendingCMD = CMD_CLOSE;
}
void requestToggle(){
    if (gateState == GATE_ERROR) {
       // Serial.println("GateControl: ERROR state, toggle ignored");
        return;
    }

    if (gateState == GATE_OPEN || gateState == GATE_OPENING) requestClose();
    else if (gateState == GATE_CLOSED || gateState == GATE_CLOSING) requestOpen();
    else requestOpen();
}

GateState_t getGateState() {return gateState;}
bool isGateBusy(){ return (gateState == GATE_OPENING || gateState == GATE_CLOSING);}

// --- Internal helpers ---
static void startOpening();
static void startClosing();
static void abortMotion(const char *reason);

// --- Public init ---
void gateControlInit() {
  gateState = GATE_STOPPED;
  pendingCMD = CMD_NONE;
  rampA = Ramp();
  rampB = Ramp();

   // Safety: ensure PWMs and direction outputs are zeroed
  setEnableA(0);
  setEnableB(0);

  actuatorAStop();
  actuatorBStop();

  Serial.println("GateControl initialized");
}

// --- Ramp updater ----
static void updateRamps(){
    uint32_t now = millis();

// A
    if (rampA.active && (now - rampA.lastMillis > RAMP_INTERVAL_MS)){
        rampA.lastMillis = now;
        if (rampA.rampUp){
            uint32_t v = (uint32_t)rampA.value + RAMP_STEP;
            if (v >= RAMP_TARGET) {rampA.value = RAMP_TARGET; rampA.active = false;}
            else rampA.value = (uint16_t)v;
        } else { //ramp down
            if (rampA.value <= RAMP_STEP){rampA.value = 0; rampA.active = false;}
        else rampA.value = rampA.value - RAMP_STEP;
    }
    setEnableA(rampA.value);
    }
    
// B
    if (rampB.active && (now - rampB.lastMillis >= RAMP_INTERVAL_MS)) {
        rampB.lastMillis = now;
        if (rampB.rampUp) {
            uint32_t v = (uint32_t)rampB.value + RAMP_STEP;
            if (v >= RAMP_TARGET) { rampB.value = RAMP_TARGET; rampB.active = false; }
            else rampB.value = (uint16_t)v;
        } else { // ramp down
            if (rampB.value <= RAMP_STEP) { rampB.value = 0; rampB.active = false; }
            else rampB.value = rampB.value - RAMP_STEP;
        }
        setEnableB(rampB.value);
    }
}

static void startOpening(){
    if (gateState == GATE_ERROR) {
    //Serial.println("GateControl: ERROR state, reset required");
    return;
}
    // if already open or opening -> ignore 
    if (gateState == GATE_OPEN || gateState == GATE_OPENING) return;

    // Safety: if limit switches indicate open already, set state
    if (digitalRead(SWA_end) == HIGH && digitalRead(SWB_end) == HIGH){
        gateState = GATE_OPEN;
        Serial.println("Gate already OPEN (limit Switches).");
        return;
    }
      Serial.println("GateControl: start opening");

  // Set direction outputs (RPWM/LPWM) using actuator functions
  actuatorAForward();   // sets RPWM_A=4095, LPWM_A=0
  actuatorBForward();

  // Initialize ramp for ENs (ramp up)
  rampA.active = true; rampA.rampUp = true; rampA.value = 0; rampA.lastMillis = millis();
  rampB.active = true; rampB.rampUp = true; rampB.value = 0; rampB.lastMillis = millis();

  // ensure EN start at 0
  setEnableA(0);
  setEnableB(0);

  // bookkeep
  gateState = GATE_OPENING;
  operationStart = millis();
}

static void startClosing() {
    if (gateState == GATE_ERROR) {
    //Serial.println("GateControl: ERROR state, reset required");
    return;
}
  if (gateState == GATE_CLOSED || gateState == GATE_CLOSING) return;

  // Safety: if limit switches indicate closed already, set state
  if (digitalRead(SWA_start) == HIGH && digitalRead(SWB_start) == HIGH) {
    gateState = GATE_CLOSED;
    Serial.println("Gate already CLOSED (limit switches).");
    return;
  }

  Serial.println("GateControl: start closing");

  actuatorABackward();
  actuatorBBackward();

  rampA.active = true; rampA.rampUp = true; rampA.value = 0; rampA.lastMillis = millis();
  rampB.active = true; rampB.rampUp = true; rampB.value = 0; rampB.lastMillis = millis();

  setEnableA(0);
  setEnableB(0);

  gateState = GATE_CLOSING;
  operationStart = millis();
}
// Abort motion and enter error/stop state
static void abortMotion(const char *reason) {
  Serial.print("GateControl: ABORT motion - ");
  Serial.println(reason);

  // ramp down quickly
  rampA.active = true; rampA.rampUp = false; rampA.lastMillis = millis();
  rampB.active = true; rampB.rampUp = false; rampB.lastMillis = millis();

  // ustaw stan ERROR
  gateState = GATE_ERROR;
  pendingCMD = CMD_NONE;

  // zapisujemy czas wejścia w ERROR
  errorStartTime = millis();
}

// --- Evaluate end conditions while moving ---
static void checkMovementCompletion() {
  if (gateState == GATE_OPENING) {
    // check current trips
    if (isCurrentTripA() || isCurrentTripB()) {
      abortMotion("current trip during opening");
      return;
    }
// check limit switches 
 if(digitalRead(SWA_end) == HIGH && digitalRead(SWB_end) == HIGH){
    // finish: ramp down then stop actuators
      rampA.active = true; rampA.rampUp = false; rampA.lastMillis = millis();
      rampB.active = true; rampB.rampUp = false; rampB.lastMillis = millis();
      gateState = GATE_OPEN; // optimistic, real stop occurs when ramps go to 0
      Serial.println("GateControl: reached OPEN limit switches");
      // clear any pending command
      pendingCMD = CMD_NONE;
      return;
 }
}
  
  else if (gateState == GATE_CLOSING) {
    if (isCurrentTripA() || isCurrentTripB()) {
      abortMotion("current trip during closing");
      return;
    }
    if (digitalRead(SWA_start) == HIGH && digitalRead(SWB_start) == HIGH) {
      rampA.active = true; rampA.rampUp = false; rampA.lastMillis = millis();
      rampB.active = true; rampB.rampUp = false; rampB.lastMillis = millis();
      gateState = GATE_CLOSED;
      Serial.println("GateControl: reached CLOSED limit switches");
      pendingCMD = CMD_NONE;
      return;
    }
  }
    // Timeout safety
  if ((gateState == GATE_OPENING || gateState == GATE_CLOSING) &&
      (millis() - operationStart > OPERATION_TIMEOUT_MS)) {
    abortMotion("operation timeout");
    return;
  }
}

// finalize ramp down 
static void finalizeStops(){
    if (gateState == GATE_OPEN || gateState == GATE_CLOSED || gateState == GATE_ERROR){
        if (rampA.value == 0 && rampB.value == 0){
            actuatorAStop();
            actuatorBStop();

            if (gateState == GATE_OPEN){
                Serial.println("GateControl: OPEN - actuators stopped");
            } else if (gateState == GATE_CLOSED){
                Serial.println("GateControl: CLOSED - actuators stopped");
            } else if (gateState == GATE_ERROR){
                static bool printed = false;
                if(!printed){
                Serial.println("GateControl: ERROR - actuators stopped");
                printed = true;
                }
            }
        }
    }
}

// main loop to be called often from main loop()
void gateControlLoop(){
    if (gateState == GATE_ERROR) {
    if (millis() - errorStartTime >= ERROR_RESET_DELAY_MS) {
        gateState = GATE_STOPPED;
        pendingCMD = CMD_NONE;
        Serial.println("GateControl: auto-reset from ERROR");
    }
}
    //1 Process pending commands 
    if(pendingCMD == CMD_OPEN){
        //if gate is busy opening, ignore; if cloasing we could optionally reverse - here we block reverse to avoid mechanical stress
        if(gateState == GATE_CLOSING){
            Serial.println("Open request ignored: gate closing (to avoid reverse)");
            pendingCMD = CMD_NONE;
        } else {
            startOpening();
            pendingCMD = CMD_NONE;
        }
    } else if (pendingCMD == CMD_CLOSE){
        if(gateState == GATE_OPENING){
            Serial.println("Close request ignored: gate opening (to avoid reverse).");
            pendingCMD = CMD_NONE;
        } else {
            startClosing();
            pendingCMD = CMD_NONE;
        } 
    }
    //2 Update ramp PWM (soft start / soft stop)
    updateRamps();
    //3 Monitor movement completion / safty conditions
    checkMovementCompletion();
    //4 When ramps reached 0 after finishing, ensure actuators stopped
    finalizeStops();
    //5 always call protection check - keep behaviour non-blocking
    //(assumes CurrentProtection module updates itself in main loop or I2C reads)
     // If current trip occurs outside movement, we still should stop actuators
    if (!isGateBusy() && (isCurrentTripA() || isCurrentTripB())) {
        abortMotion("current trip while idle");
    }
}