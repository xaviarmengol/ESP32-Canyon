#pragma once

//#include "Arduino.h"
#include "SharedVar.hpp"
#include "AsyncTask.hpp"


// Aplication specific
enum ModeRun {STOP, HOME, MAN, AUTO, LASER, TOTAL_ModeRun};

// Task management

extern AsyncTask tasksManager[TaskNames::TOTAL_TASK];
extern SharedVar<bool> gTaskSetupFinished[TaskNames::TOTAL_TASK];
extern SharedVar<int> gPeriodBusy[TOTAL_TASK];


// Global variables

extern SharedVar<int> gModeNum;

extern SharedVar<int> gInpElev;
extern SharedVar<int> gInpRot;
extern SharedVar<int> gInpFire;

extern SharedVar<int> gOutElev;
extern SharedVar<int> gOutRot;

extern SharedVar<int> gLidarDist;
extern SharedVar<int> gValLog;

// Calibration

extern SharedVar<int> gCalRot; //=-16;
extern SharedVar<int> gCalElev; //=-4;
extern SharedVar<int> gCalFire;
extern SharedVar<int> gCalv0; //=478;
extern SharedVar<int> gCalHardOffset; //=90;

// Target management

extern SharedVar<int> gXTarget; //=0;
extern SharedVar<int> gYTarget; //=0;
extern SharedVar<int> gXTargetLidar; //=0;
extern SharedVar<int> gYTargetLidar; //=0;
extern SharedVar<int> gAngleTarget; //=0;
extern SharedVar<int> gRotTarget; //=0;
extern SharedVar<int> gXAuto; //=120;
extern SharedVar<int> gYAuto; //7=-11;
extern SharedVar<bool> gHighTraj; //=1; // 1 if high, 0 if low


extern SharedVar<bool> gFireAllowed;
extern SharedVar<bool> gCalibrate;





