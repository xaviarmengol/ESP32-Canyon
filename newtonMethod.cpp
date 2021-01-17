#include <Arduino.h>
#include "newtonMethod.hpp"

using namespace std;

// Input cm
// Output degrees

int newtonMethod(int posXInt, int posYInt, float v0, int angleIni, bool& solutionFound) {

    double prevA; 
    double a;
    double precision;
    solutionFound=false;

    int returnedAngle;

    double posX;
    double posY;

    posX = (float)posXInt / 100.0;
    posY = (float)posYInt / 100.0;

    prevA = angleIni * (PI / 180.0) ;
    
    precision = 1 * (PI / 180.0);

    for (int i=0; i<30; i++) {

        a = prevA - (function(prevA, posX, posY, v0) / functionDerivative(prevA, posX, posY, v0));
        prevA = a;

        //Serial.println("X: " + String(a) + " f(a): " + String(function(a, posX, posY, v0)));
        if (abs(function(a, posX, posY, v0)) < precision) {
            solutionFound=true;
            break;
        }
    }

    returnedAngle = ((int)round(a * 180.0 / PI)) % 360;
    if (returnedAngle<-270) returnedAngle+=360;
    if (returnedAngle>270) returnedAngle-=360;

    if (returnedAngle > 90 || returnedAngle<-90) solutionFound = false;
    
    return (returnedAngle);

}

double function(double a, double posX, double posY, double v0) {
    //return (v0*sin(a) - (9.8/2.0) * pow(posX  / (v0*cos(a)), 2) - posY);

    return(posX * tan(a) - (9.8*posX*posX)/(2*v0*v0*cos(a)*cos(a)) - posY);

}

double functionDerivative(double a, double posX, double posY, double v0) {
    //return (v0*cos(a) - (9.8/2.0) * pow((posX/v0), 2) * ( (2 * sin(a)) / pow(cos(a), 3) ) );

    return (posX*(1/(cos(a)*cos(a))) - ((9.8 * posX * posX ) / (2 * v0 * v0)) * ((2 * sin(a)) / (cos(a)*cos(a)*cos(a))) );
}