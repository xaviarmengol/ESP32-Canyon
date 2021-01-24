#include <Arduino.h>
#include "configBlynk.h"
#include "wifiUtils.h"
#include "configGlobalVars.hpp"


void tskWifi(void *pvParameters) {

    Serial.println("Task Wifi on core: " + String(xPortGetCoreID()));
    TaskNames taskName = TaskNames::WIFI;
    int taskId = static_cast<int>(taskName);
    unsigned long periods=0;
    
    InitWifi();

    Blynk.config(auth);
    //Blynk.config(localAuth, localServer, 8080); 
    //Blynk.connect();

    // Tasks Setup
    tasksManager[taskId].modifyWaitMs(300);
    
    gTaskSetupFinished[taskId].set(true);

    while(true) {

        if (tasksManager[taskId].hasPeriodElapsedWithExternalTimer()) {

            if ((periods % 1) == 0 ) {
                Blynk.virtualWrite(V6, gLidarDist.get());
                Blynk.virtualWrite(V7, gAngleTarget.get());
            }

            Blynk.run();
            periods++;

            gPeriodBusy[taskId].set(tasksManager[taskId].getBusyPercentage());

            //Serial.println("Period WIFI");

        } else { // Message management

            asyncMsg_t msg = tasksManager[taskId].getLastValidAsyncMsg();
            Serial.print("Executing Message: ");
            Serial.println(static_cast<int>(msg));

            if (msg == AsyncMsg::RESET_CALIB) {
                // When forcing a value, both Blynk and local value should be changed

                gInpRot.set(0);
                gInpElev.set(0); 
                gCalibrate.set(0);

                Blynk.virtualWrite(V0, 0); // Home
                Blynk.virtualWrite(V1, 0); // Home
                Blynk.virtualWrite(V10,0);
            }

        }

    }

}

