#include <Arduino.h>
#include "configBlynk.h"
#include "wifiUtils.h"
#include "configGlobalVars.hpp"

void asyncMsgCallBack(asyncMsg_t msg) {
    Serial.print("Executing Message: ");
    Serial.println((int)msg);
    if (msg == asyncMsg_t::RESET_CALIB) {
        // When forcing a value, both Blynk and local value should be changed

        gInpRot.set(0);
        gInpElev.set(0); 
        gCalibrate.set(0);

        Blynk.virtualWrite(V0, 0); // Home
        Blynk.virtualWrite(V1, 0); // Home
        Blynk.virtualWrite(V10,0);
    }
} 

void tskWifi(void *pvParameters){
    Serial.println("Task Wifi on core: " + String(xPortGetCoreID()));
    int taskId = static_cast<int>(TaskNames::WIFI);
    unsigned long periods=0;
    
    InitWifi();

    Blynk.config(auth);
    //Blynk.config(localAuth, localServer, 8080); 
    //Blynk.connect();

    // Tasks Setup
    tasksManager[taskId].setAsyncMsgCallBack(&asyncMsgCallBack);
    tasksManager[taskId].modifyWaitMs(500);
    
    gTaskSetupFinished[taskId].set(true);

    while(true){
        if(tasksManager[taskId].hasPeriodElapsed()) {
            if ((periods % 1) == 0 ) {
                Blynk.virtualWrite(V6, gLidarDist.get());
                Blynk.virtualWrite(V7, gAngleTarget.get());
            }

            Blynk.run();
            periods++;

            gPeriodBusy[taskId].set(tasksManager[taskId].getBusyPercentage());
        }
    }
}

