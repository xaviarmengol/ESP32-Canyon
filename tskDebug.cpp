#include <Arduino.h>

#include "configPinOut.hpp"
#include "configGlobalVars.hpp"

#include "AsyncTaskMsg.hpp"


void tskDebug (void *pvParameters){

    // SETUP or the task
    Serial.println("Task Degug Loop on core: " + String(xPortGetCoreID()));
    TaskNames taskName = TaskNames::DEBUG;
    int taskId = static_cast<int>(taskName);
    AsyncTaskMsg localData(taskName);

    bool blinkState = false;

    tasksManager[taskId].modifyWaitMs(1000);

    gTaskSetupFinished[taskId].set(true);

    while(1) {        
        if(tasksManager[taskId].periodEnded()) {
            //gLidarDist.print("Lidar");
            gInpElev.print("Inp Elev");
            gInpRot.print("Inp Rot");
            //gOutElev.print("Elev Hard");
            //gOutRot.print("Rot Hard");
            //gCalElev.print("Cal Elev");
            //gCalRot.print("Cal Rot");
            //gModeNum.print("Mode");
            for (int i=0; i<TOTAL_TASK;i++) gPeriodBusy[i].print(String("Task ") + String(i));
            Serial.print(" | ");
            Serial.print(esp_get_free_heap_size());
            Serial.println(" | ");

            // Blinking PIN
            blinkState = !blinkState;
            digitalWrite(DEBUG_PIN, (int)blinkState);

            gPeriodBusy[taskId].set(tasksManager[taskId].getBusyPercentage());
        } else {
            localData.update();

            Serial.print("Debug Message received: ");
            Serial.print((int)tasksManager[taskId].getMsgName());
            Serial.print(" at ");
            Serial.println(millis());

        }
    }

}
