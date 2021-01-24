#include <BlynkSimpleEsp32_SSL.h>
#include "configGlobalVars.hpp"

#define BLYNK_PRINT Serial // Defines the object that is used for printing
#define BLYNK_DEBUG // Optional, this enables more detailed prints


BLYNK_WRITE(V0) { // Joystick
    gInpElev.set(param.asInt());
}

BLYNK_WRITE(V1) { // Joystick
    gInpRot.set(param.asInt());
}

BLYNK_WRITE(V2) { // Fire
    gInpFire.set(param.asInt());
}

BLYNK_WRITE(V3) { // Mode (starts at 1)
    gModeNum.set(param.asInt() - 1);
}

BLYNK_WRITE(V4) { // X Auto 
    gXAuto.set(param.asInt());
}

BLYNK_WRITE(V5) { // Y Auto
    gYAuto.set(param.asInt());
}

BLYNK_WRITE(V8) { // High Trajectory
    gHighTraj.set(param.asInt());
}

BLYNK_WRITE(V10) { // Is calibrated
    gCalibrate.set((bool)param.asInt());
}

BLYNK_WRITE(V11) { // Fire Calibration
    gCalFire.set(param.asInt());
}

BLYNK_WRITE(V12) { // V0 calibration
    gCalv0.set(param.asInt());
}
