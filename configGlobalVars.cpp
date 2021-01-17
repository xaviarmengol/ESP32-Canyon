#include "configGlobalVars.hpp"
#include "SharedVar.hpp"
#include "AsyncTask.hpp"

// Task managment

AsyncTask tasksManager[TOTAL_TASK];
SharedVar<bool> gTaskSetupFinished[TOTAL_TASK];
SharedVar<int> gPeriodBusy[TOTAL_TASK];

// Global Variables

SharedVar<int> gModeNum;

SharedVar<int> gInpElev;
SharedVar<int> gInpRot;
SharedVar<int> gInpFire;

SharedVar<int> gOutElev;
SharedVar<int> gOutRot;

SharedVar<int> gLidarDist;
SharedVar<int> gValLog;

// Calibration

SharedVar<int> gCalRot; //=-16;
SharedVar<int> gCalElev; //=-4;
SharedVar<int> gCalFire;
SharedVar<int> gCalv0; //=478;
SharedVar<int> gCalHardOffset; //=90;

// Target management

SharedVar<int> gXTarget; //=0;
SharedVar<int> gYTarget; //=0;
SharedVar<int> gXTargetLidar; //=0;
SharedVar<int> gYTargetLidar; //=0;
SharedVar<int> gAngleTarget; //=0;
SharedVar<int> gRotTarget; //=0;
SharedVar<int> gXAuto; //=120;
SharedVar<int> gYAuto; //7=-11;
SharedVar<int> gHighTraj; //=1; // 1 if high, 0 if low

SharedVar<bool> gFireAllowed;
SharedVar<bool> gCalibrate;





