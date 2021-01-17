#include <Arduino.h>

#include "configPinOut.hpp"
#include "configGlobalVars.hpp"

#include "CtrlServo.hpp"
#include "CtrlTOF10120.hpp"
#include "newtonMethod.hpp"

#include "FlankFilter.hpp"

constexpr int POSITION_SERVO_LOW = 60;
constexpr int POSITION_SERVO_HIGH = 82;

// TODO: Could be static inside the State Machine
enum struct ModeFire {search, toFire, fire, toSeach};

void stateMachine();
void fireControlCalc();
void calibrateServos();
void calcTargetXY (int angle, int dist);

servoControl servoRot;
servoControl servoElev;
servoControl servoFire;

FlankFilter flankCalibrate(true);

void tskPosition(void *pvParameters){

    Wire.begin(); 

    // SETUP or the task
    Serial.println("Task Position on core: " + String(xPortGetCoreID()));
    int taskId = static_cast<int>(TaskNames::POSITION);

    int32_t periodMs = 20;

    servoElev.attach(PIN_SERVO_ELEV);
    servoRot.attach(PIN_SERVO_ROT);
    servoFire.attach(PIN_SERVO_FIRE);

    calibrateServos();

    TOF10120 lidar;
    int lidarDist;

    tasksManager[taskId].modifyWaitMs(periodMs);

    gTaskSetupFinished[taskId].set(true);
   
    while(1) {

        if(tasksManager[taskId].hasPeriodElapsed()) {
            lidar.update();
            lidarDist = lidar.getDistance() / 10; // cm
            //Serial.println(lidarDist);
            gLidarDist.set(lidarDist);

            calcTargetXY(gInpElev.get(), lidarDist);

            fireControlCalc();

            stateMachine();

            gPeriodBusy[taskId].set(tasksManager[taskId].getBusyPercentage());
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



void calcTargetXY (int angle, int dist) {

    gXTargetLidar.set((int)(cos(angle * (PI / 180)) * dist));

    gYTargetLidar.set((int)(sin(angle * (PI / 180)) * dist));

}



void fireControlCalc() {

    int xTarget = gXTarget.get();
    int yTarget = gYTarget.get();
    int modeNum = gModeNum.get();
    
    int xTargetLidar = gXTargetLidar.get();
    int yTargetLidar = gYTargetLidar.get();

    int angleTarget = gAngleTarget.get();
    int lidarDist = gLidarDist.get();

    int calv0 = gCalv0.get();

    int xAuto = gXAuto.get();
    int yAuto = gYAuto.get();

    bool fireAllowed = gFireAllowed.get();

    int solIni;
    bool solutionFound;

    if (gHighTraj.get() == 1) solIni=85;
    else solIni=15;

    if (modeNum == ModeRun::AUTO) {
        xTarget = xAuto;
        yTarget = yAuto;

    } if (modeNum == ModeRun::LASER) {
        xTarget = xTargetLidar;
        yTarget = yTargetLidar;
    }

    angleTarget = newtonMethod(xTarget, yTarget, (float)(calv0/100.0), solIni, solutionFound);

    if (!solutionFound) angleTarget = 0;
    fireAllowed = solutionFound && (lidarDist<=195 || modeNum==3);

    gAngleTarget.set(angleTarget);
    gFireAllowed.set(fireAllowed);

    gXTarget.set(xTarget);
    gYTarget.set(yTarget);
         
}



void stateMachine() {

    int modeNum = gModeNum.get();
    int inpRot = gInpRot.get();
    int inpElev = gInpElev.get();
    int inpFire = gInpFire.get();
    int rotTarget = gRotTarget.get();
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

            tasksManager[taskNames_t::WIFI].sendMessage(asyncMsg_t::RESET_CALIB);            
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

                    rotDest = rotTarget;
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
                modeFire=ModeFire::toSeach;
                millisToSearch = millis();
                inpFire=0;
            }
            
        } if (modeFire == ModeFire::toSeach) {

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

