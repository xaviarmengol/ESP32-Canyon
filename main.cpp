
#include <Arduino.h>

#include "configPinOut.hpp"
#include "configGlobalVars.hpp"

void waitUntilSetupEnds(int tskNum);

// Task Periodic timer to fire Period Timers
void tskPeriodicTimer (void *pvParameters);

// Task fordward declaration. Can be in a hpp file
void tskPosition (void *pvParameters);
void tskWifi (void *pvParameters);
void tskDebug (void *pvParameters);


void setup() {

    Serial.begin(115200);

    pinModeConfig();
    
    // Initialize global variables

    // Calibration
    gCalRot.set(90);
    gCalElev.set(90);
    gCalFire.set(-10);
    gCalv0.set(478);
    gCalHardOffset.set(0);

    gCalibrate.set(false);

    // Target management
    gXAuto.set(120);
    gYAuto.set(-11);
    gHighTraj.set(1); // 1 if high, 0 if low

    // Mode management
    gModeNum.set(ModeRun::STOP);

    // Periodic timer
    TaskHandle_t periodicTimerHandler;

    // Initialitze Async Queues object
    for (int i=0; i<TaskNames::TOTAL_TASK; i++) {
        tasksManager[i] = AsyncTask((taskNames_t)i, 1000); // Create objects
    }

    // TODO: Understant what is the best core asignment strategy

    // Wifi, first task to setup and in core 1. If not, it crash
    xTaskCreateUniversal(tskWifi, "TaskWifi", 10000, NULL, 0, tasksManager[WIFI].getTaskHandler(), 1);
    waitUntilSetupEnds(WIFI);
    
    // Control tsk in 0, others in 1 (Arduino)
    xTaskCreateUniversal(tskPosition, "TskPosition", 10000, NULL, 3, tasksManager[POSITION].getTaskHandler(), 0);
    waitUntilSetupEnds(POSITION);
    
    xTaskCreateUniversal(tskDebug, "TaskDebug", 10000, NULL, 0, tasksManager[DEBUG].getTaskHandler(), 1);
    waitUntilSetupEnds(DEBUG);

    // Periodic timer fires messages at some freq.

    xTaskCreateUniversal(tskPeriodicTimer, "PERIODIC_TIMER", 10000, NULL, 0, &periodicTimerHandler, 1);
    waitUntilSetupEnds(DEBUG);

    Serial.println("Setup end");
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(10000)); // Do nothing
}

void waitUntilSetupEnds(int tskNum) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    while (!gTaskSetupFinished[tskNum].get()) {}
    Serial.print("Task setup finished: ");Serial.println(tskNum);
}

