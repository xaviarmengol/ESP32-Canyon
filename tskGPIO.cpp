#include <Arduino.h>
#include <array>

#include <Servo.h>

#include "configPinOut.hpp"
#include "configGlobalVars.hpp"

// Object to manage async message between tasks
#include "AsyncTaskMsg.hpp"

void tskGPIO (void *pvParameters){
    // SETUP or the task
    Serial.println("Task GPIO Loop on core: " + String(xPortGetCoreID()));
    TaskNames taskName = TaskNames::HARD_IO;
    int taskId = static_cast<int>(taskName);
    tasksManager[taskId].modifyWaitMs(10000);
    AsyncTaskMsg async(taskName);

    Servo servo[3];

    servo[0].attach(PIN_SERVO_ELEV);
    servo[1].attach(PIN_SERVO_ROT);
    servo[2].attach(PIN_SERVO_FIRE);

    gTaskSetupFinished[taskId].set(true);

    while(1) {

        if(tasksManager[taskId].periodEnded()) {

        } else {
            async.update();
            msgQueue_t lastMessage = tasksManager[taskId].getLastQueueMsg();

            if (lastMessage.msg == AsyncMsg::WRITE_SERVO) {
                //Write to PWM lastMessage.index the value lastMessage.value
                servo[lastMessage.index].write(lastMessage.value);
            }
        }
    }
}

