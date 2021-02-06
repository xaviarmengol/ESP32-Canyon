#include <Arduino.h>
#include <array>

#include "configGlobalVars.hpp"
#include "configTasks.hpp"


void tskPeriodicTimer (void *pvParameters){
    
    // SETUP or the task
    Serial.println("Task Timer on core: " + String(xPortGetCoreID()));
    TaskNames taskName = TaskNames::_TIMER;
    std::array<unsigned long, TOTAL_TASK> taskPeriod;
    
    // Initialize periods array with the info in Task Manager
    for (int i=0; i<TOTAL_TASK; i++) {
        taskPeriod.at(i) = tasksManager[i].getWaitMs();
    }

    int numTasks = taskPeriod.size();

    std::array<unsigned long, TOTAL_TASK> timeToFinishPeriod;
    timeToFinishPeriod = taskPeriod;

    TickType_t lastTime = 0;
    unsigned long delayTime = 0;


    while(1) {

        int minIndex = 0;
        for (int i=1; i<numTasks; i++) {
            if (timeToFinishPeriod.at(i) < timeToFinishPeriod.at(minIndex)) minIndex = i;
        }
        delayTime = timeToFinishPeriod.at(minIndex);

        // Delay time
        vTaskDelay(pdMS_TO_TICKS(delayTime));
        //vTaskDelayUntil(&lastTime, pdMS_TO_TICKS(delayTime));

        // Send a message to the task (Value = Period Time)
        //Serial.print("Sending MinIndex: ");
        //Serial.println(minIndex);
        tasksManager[minIndex].sendMessage(AsyncMsg::PERIOD_TIME_OUT, taskName, 0, taskPeriod.at(minIndex) );

        if (delayTime > 0) {
            for (int i=0;i<numTasks;i++) {

                if (timeToFinishPeriod.at(i) >= delayTime) {
                    timeToFinishPeriod.at(i) = timeToFinishPeriod.at(i) - delayTime;
                } else  {
                    timeToFinishPeriod.at(i) = 0;
                }
            }
        }

        timeToFinishPeriod.at(minIndex) = taskPeriod.at(minIndex); 

    }
}
