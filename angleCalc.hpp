//http://www.sc.ehu.es/sbweb/fisica3/cinematica/curvilineo/curvilineo_3.html

#include <Arduino.h>

int normalizeAngle(int angle);

bool angleCalculation(int velIni, int xDest, int yDest, bool highAngle, int& outSol) {
    bool angleExist = false;

    double v0 = static_cast<double>(velIni) / 100.0; // Divided by 100 because is cm/s
    double x = static_cast<double>(xDest) / 100.0; // Divided by 100 because is cm
    double y = static_cast<double>(yDest) / 100.0; // Divided by 100 becaus is cm

    double v02 = pow(v0, 2);
    double x2 = pow(x,2);

    double g = 9.8; //aceleración de la gravedad
    double discriminant = pow(v02/(g*x),2) -1 -(2*v02*y/(g*x2));

    if (discriminant >=0 ) {
        double sqrRootDiscriminant = sqrt(discriminant); 
        angleExist = true;

        double sol1 = atan(v02/(g*x) + sqrRootDiscriminant);
        double sol2 = atan(v02/(g*x) - sqrRootDiscriminant);

        int sol1Deg = normalizeAngle(static_cast<int>(sol1 * RAD_TO_DEG));
        int sol2Deg = normalizeAngle(static_cast<int>(sol2 * RAD_TO_DEG));

        outSol = highAngle ? max(sol1Deg, sol2Deg) : min(sol1Deg, sol2Deg);
    } else {
        angleExist = false;
    }
    return (angleExist);
}

int normalizeAngle(int angle) {
    int retAngle = angle;
    
    if (retAngle > 90) { 
        retAngle -= 360;
    } else if (retAngle < -90) {
        retAngle += 360;
    }

    return(retAngle);
}