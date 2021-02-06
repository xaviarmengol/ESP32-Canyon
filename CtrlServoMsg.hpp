#include <Arduino.h>
#include "configGlobalVars.hpp"

// Facade that sends messages to the I2C and Hardware tasks instead of handling directly the hardware

class servoControl {

private:
    
    int homeCalibration=0;
    int maxVal=180-20;
    int minVal=20;
    int lastAngle=0;
    int lastHardAngle=0;
    int offset=0;
    bool invert=false;

    int indexServo;
    taskNames_t taskName;
    bool msgI2C = false;

    bool lastMsgI2C = false;
    int lastValue = 0;

    int calcHardwareAngle (int angle);

    int lowAngle=-45;
    int highAngle=45;

    void sendMsgServoWrite(int value, bool I2C=false);

public:
    servoControl();
    void attachIndex(int index, taskNames_t task, bool msgI2CPar = false);
    void write(int angle);
    void calibrateFromLastHardAngle();
    void writeTwoModes(int angleBool);
    void setHomeCalibration(int cal);
    void setHardwareOffset(int offset);
    void setInvert(bool invertPar);
    void writeMax();
    void writeMin();
    void writeHome();
    int getAngle();
    int getHardwareAngle();

    // two positions servo

    void setTwoPositionsMode(int low, int high);
    
};

servoControl::servoControl() {

}

void servoControl::attachIndex(int indexPar, taskNames_t task, bool msgI2CPar){
    indexServo = indexPar;
    taskName = task;
    msgI2C = msgI2CPar;

}

int servoControl::calcHardwareAngle (int angle) {
    int angleCalc = (angle ) * (invert ? -1 : 1) + homeCalibration;

    return(constrain( angleCalc + offset, minVal, maxVal));
}

void servoControl::write(int angle){
    lastAngle = angle;

    int hardAngle = calcHardwareAngle(angle);
    lastHardAngle = hardAngle;
    sendMsgServoWrite(hardAngle, msgI2C);
}

void servoControl::writeTwoModes(int angleBool){

    int hardAngle;
    hardAngle = (angleBool==0) ? calcHardwareAngle(lowAngle) : calcHardwareAngle(highAngle);
    lastHardAngle = hardAngle;
    sendMsgServoWrite(hardAngle, msgI2C);
    //Serial.println(hardAngle);
}

void servoControl::setHardwareOffset(int offsetPar) {
    offset = offsetPar;
}


void servoControl::setHomeCalibration(int cal) {
    homeCalibration = cal;
}

void servoControl::calibrateFromLastHardAngle() {
    homeCalibration = getHardwareAngle();
}


void servoControl::setInvert(bool invertPar) {
    invert = invertPar;
}

void servoControl::writeMax(){
    sendMsgServoWrite(maxVal, msgI2C);
}

void servoControl::writeMin(){
    sendMsgServoWrite(minVal, msgI2C);
}

void servoControl::writeHome(){
    sendMsgServoWrite(calcHardwareAngle(0), msgI2C);
}

int servoControl::getAngle(){
    return(lastAngle);
}

int servoControl::getHardwareAngle(){
    return(lastHardAngle);
}

void servoControl::setTwoPositionsMode(int low, int high) {
    lowAngle = low;
    highAngle = high;
}

void servoControl::sendMsgServoWrite(int value, bool I2Cpar) {

    if (lastValue != value or lastMsgI2C != I2Cpar) {
        taskNames_t taskDest = I2Cpar ? TaskNames::I2C : TaskNames::HARD_IO;
        tasksManager[taskDest].sendMessage(AsyncMsg::WRITE_SERVO, taskName, indexServo, value);

        lastValue = value;
        lastMsgI2C = I2Cpar;
    }
}
