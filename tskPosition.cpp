#include <Arduino.h>

#include "configGlobalVars.hpp"
#include "AsyncTaskMsg.hpp"
#include "CtrlServoMsg.hpp"

#include "angleCalc.hpp"
#include "FlankFilter.hpp"

constexpr int POSITION_SERVO_LOW = 60;
constexpr int POSITION_SERVO_HIGH = 82;

// TODO: Could be static inside the State Machine
enum struct ModeFire {search, toFire, fire, toSearch};

void stateMachine(taskNames_t taskName);
void fireControl(int modeNum, int lidarDist);
void calibrateServos();
void calcTargetXY (int angle, int dist);

servoControl servoRot;
servoControl servoElev;
servoControl servoFire;

FlankFilter flankCalibrate(true);

void tskPosition(void *pvParameters){

    //Wire.begin(); 

    // SETUP or the task
    Serial.println("Task Position on core: " + String(xPortGetCoreID()));
    TaskNames taskName = TaskNames::POSITION;
    int taskId = static_cast<int>(taskName);
    AsyncTaskMsg async(taskName);

    int32_t periodMs = 20;

    servoElev.attachIndex(0, taskName, true);
    servoRot.attachIndex(1, taskName, true);
    servoFire.attachIndex(2, taskName, true);

    calibrateServos();

    tasksManager[taskId].modifyWaitMs(periodMs);

    gTaskSetupFinished[taskId].set(true);
   
    while(1) {

        if(tasksManager[taskId].periodEnded()) {
            fireControl(gModeNum.get(), gLidarDist.get());
            stateMachine(taskName);
            
            gPeriodBusy[taskId].set(tasksManager[taskId].getBusyPercentage());
            
        } else {
            async.update();

        }
    }

}


void calibrateServos() {

    servoElev.setHardwareOffset(gCalHardOffset.get());
    servoElev.setHomeCalibration(gCalElev.get());
    servoElev.setInvert(false);

    servoRot.setHardwareOffset(gCalHardOffset.get());
    servoRot.setHomeCalibration(gCalRot.get());
    servoRot.setInvert(false);

    servoFire.setTwoPositionsMode(POSITION_SERVO_LOW + gCalFire.get(), POSITION_SERVO_HIGH + gCalFire.get());
    
}


void fireControl(int modeNum, int lidarDist) {
    
    if (modeNum == ModeRun::AUTO or modeNum == ModeRun::LASER) {

        int xTarget = 0;
        int yTarget = 0;

        if (modeNum == ModeRun::AUTO) {
            xTarget = gXAuto.get();
            yTarget = gYAuto.get();
        } else if (modeNum == ModeRun::LASER) {
            int inpElev = gInpElev.get();
            xTarget = (int)(cos(inpElev * DEG_TO_RAD ) * lidarDist);
            yTarget = (int)(sin(inpElev * DEG_TO_RAD ) * lidarDist);
        }

        

        int solIni;
        bool solutionFound;
        int angleTarget = 0;

        /*
        if (gHighTraj.get() == 1) solIni=85; else solIni=15;
        int calv0 = gCalv0.get();
        angleTarget = newtonMethod(xTarget, yTarget, (float)(calv0/100.0), solIni, solutionFound);
        */

        solutionFound = angleCalculation(gCalv0.get(), xTarget, yTarget, gHighTraj.get(), angleTarget);
        if (!solutionFound) angleTarget = 0;
        
        bool fireAllowed = solutionFound and (lidarDist<=195 || modeNum==3);

        gXTarget.set(xTarget);
        gYTarget.set(yTarget);

        gAngleTarget.set(angleTarget);
        gFireAllowed.set(fireAllowed);

    }
         
}


void stateMachine(taskNames_t taskName) {

    int modeNum = gModeNum.get();
    int inpRot = gInpRot.get();
    int inpElev = gInpElev.get();
    int inpFire = gInpFire.get();
    //int rotTarget = gRotTarget.get();
    int angleTarget = gAngleTarget.get();
    bool fireAllowed = gFireAllowed.get();

    // Static because value should be mantained from call to call

    static unsigned long millisToSearch;
    static unsigned long millisToFire;
    static unsigned long millisFire;

    static int inpRotOld;
    static int inpElevOld;
    static int rotDest;
    static int elevDest;

    static ModeFire modeFire = ModeFire::search;

    if (modeNum==ModeRun::STOP) { // Stop
        
    } else if (modeNum == ModeRun::HOME) { // Home

        servoElev.writeHome();
        servoRot.writeHome();
        servoFire.writeTwoModes(0);

    } else if (modeNum == ModeRun::MAN) { // Man

        servoElev.write(inpElev);
        servoRot.write(inpRot);
        servoFire.writeTwoModes(inpFire);

        if (flankCalibrate.filter(gCalibrate.get())) {

            //servoElev.calibrateFromLastHardAngle();
            //servoRot.calibrateFromLastHardAngle();

            int lastHardAngleRot = servoRot.getHardwareAngle();
            int lastHardAngleElev = servoElev.getHardwareAngle();

            gCalRot.set(lastHardAngleRot);
            gCalElev.set(lastHardAngleElev);

            servoRot.setHomeCalibration(lastHardAngleRot);
            servoElev.setHomeCalibration(lastHardAngleElev);
            
            servoElev.writeHome();
            servoRot.writeHome();

            servoFire.setTwoPositionsMode(POSITION_SERVO_LOW + gCalFire.get(), POSITION_SERVO_HIGH + gCalFire.get());

            tasksManager[taskNames_t::WIFI].sendMessage(AsyncMsg::RESET_CALIB, taskName);            
        }


    } else if (modeNum == ModeRun::AUTO || modeNum == ModeRun::LASER) { // Auto or Lidar

        if (modeFire == ModeFire::search) {

            servoElev.write(inpElev);
            servoRot.write(inpRot);
            servoFire.writeTwoModes(0);

            if(gInpFire.get()==1) {

                if (fireAllowed) {

                    modeFire = ModeFire::toFire;
                    millisToFire = millis();
                    inpRotOld = inpRot;
                    inpElevOld = inpElev;

                    // should be fixed to avoid changes during shoting phase

                    //rotDest = rotTarget;
                    rotDest = inpRot;
                    elevDest = angleTarget;

                } else {
                    modeFire = ModeFire::search;
                    inpFire=0;
                }               
            }

        } if (modeFire == ModeFire::toFire) {

            servoElev.write(elevDest);
            servoRot.write(rotDest);
            servoFire.writeTwoModes(0);
            
            if ((millis()-millisToFire) > 750) {
                modeFire=ModeFire::fire;
                millisFire = millis();
            }

        } if (modeFire == ModeFire::fire) {

            servoElev.write(elevDest);
            servoRot.write(rotDest);
            servoFire.writeTwoModes(1);

            if ((millis() - millisFire) > 250) {
                servoFire.writeTwoModes(0);
                modeFire=ModeFire::toSearch;
                millisToSearch = millis();
                inpFire=0;
            }
            
        } if (modeFire == ModeFire::toSearch) {

            servoElev.write(inpElevOld);
            servoRot.write(inpRotOld);
            servoFire.writeTwoModes(0);
            
            if ((millis() - millisToSearch) > 750) {
                modeFire=ModeFire::search;
            }

        } 

    }

    gOutElev.set(servoElev.getHardwareAngle());
    gOutRot.set(servoRot.getHardwareAngle());

}

