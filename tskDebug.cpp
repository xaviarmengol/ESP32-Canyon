#include <Arduino.h>

#include "configPinOut.hpp"
#include "configGlobalVars.hpp"

void tskDebug (void *pvParameters){

    // SETUP or the task
    Serial.println("Task Degug Loop on core: " + String(xPortGetCoreID()));
    int taskId = static_cast<int>(TaskNames::DEBUG);

    bool blinkState = false;

    tasksManager[taskId].modifyWaitMs(500);

    gTaskSetupFinished[taskId].set(true);

    while(1) {        
        if(tasksManager[taskId].hasPeriodElapsed()) {
            //gLidarDist.print("Lidar");
            gInpElev.print("Inp Elev");
            gInpRot.print("Inp Rot");
            gOutElev.print("Elev Hard");
            gOutRot.print("Rot Hard");
            gCalElev.print("Cal Elev");
            gCalRot.print("Cal Rot");
            //gModeNum.print("Mode");
            //for (int i=0; i<TOTAL_TASK;i++) gPeriodBusy[i].print(String("Task ") + String(i));
            Serial.print(" | ");
            Serial.print(esp_get_free_heap_size());
            Serial.println(" | ");

            // Blinking PIN
            blinkState = !blinkState;
            digitalWrite(DEBUG_PIN, (int)blinkState);

            gPeriodBusy[taskId].set(tasksManager[taskId].getBusyPercentage());
        }
    }

}
