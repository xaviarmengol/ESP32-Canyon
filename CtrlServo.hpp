#include <Arduino.h>
#include <Servo.h>

class servoControl {

private:
    Servo servo;
    int homeCalibration=0;
    int maxVal=160;
    int minVal=20;
    int lastAngle=0;
    int lastHardAngle=0;
    int offset=0;
    bool invert=false;

    int calcHardwareAngle (int angle);

    int lowAngle=-45;
    int highAngle=45;

public:
    servoControl();
    void attach(int port);
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

void servoControl::attach(int port){
    servo.attach(port);
    
}

int servoControl::calcHardwareAngle (int angle) {
    int angleCalc = (angle ) * (invert ? -1 : 1) + homeCalibration;

    return(constrain( angleCalc + offset, minVal, maxVal));
}

void servoControl::write(int angle){
    lastAngle = angle;

    int hardAngle = calcHardwareAngle(angle);
    lastHardAngle = hardAngle;
    servo.write(hardAngle);
}

void servoControl::writeTwoModes(int angleBool){

    int hardAngle;
    hardAngle = (angleBool==0) ? calcHardwareAngle(lowAngle) : calcHardwareAngle(highAngle);
    lastHardAngle = hardAngle;
    servo.write(hardAngle);
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
    servo.write(maxVal);
}

void servoControl::writeMin(){
    servo.write(minVal);
}

void servoControl::writeHome(){
    servo.write(calcHardwareAngle(0));
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

