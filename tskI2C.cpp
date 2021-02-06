#include <Arduino.h>
#include <array>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

//#include "configPinOut.hpp"
#include "configGlobalVars.hpp"
#include "CtrlTOF10120.hpp"

// Object to manage async message between tasks
#include "AsyncTaskMsg.hpp"

#define SERVOMIN  70 // 150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  420 // 600 // This is the 'maximum' pulse length count (out of 4096)

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

void tskI2C (void *pvParameters){
    // SETUP or the task
    Serial.println("Task I2C Loop on core: " + String(xPortGetCoreID()));
    TaskNames taskName = TaskNames::I2C;
    int taskId = static_cast<int>(taskName);
    tasksManager[taskId].modifyWaitMs(100);
    AsyncTaskMsg async(taskName);

    // Task setup (I2C)
    Wire.begin(); 
    TOF10120 lidar;
    int lidarDist;

    // PWM I2C module
    Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);
    int32_t calculatedValuePWM;


    pwm.begin();
    /*
    * In theory the internal oscillator (clock) is 25MHz but it really isn't
    * that precise. You can 'calibrate' this by tweaking this number until
    * you get the PWM update frequency you're expecting!
    * The int.osc. for the PCA9685 chip is a range between about 23-27MHz and
    * is used for calculating things like writeMicroseconds()
    * Analog servos run at ~50 Hz updates, It is importaint to use an
    * oscilloscope in setting the int.osc frequency for the I2C PCA9685 chip.
    * 1) Attach the oscilloscope to one of the PWM signal pins and ground on
    *    the I2C PCA9685 chip you are setting the value for.
    * 2) Adjust setOscillatorFrequency() until the PWM update frequency is the
    *    expected value (50Hz for most ESCs)
    * Setting the value here is specific to each individual I2C PCA9685 chip and
    * affects the calculations for the PWM update frequency. 
    * Failure to correctly set the int.osc value will cause unexpected PWM results
    */
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

    gTaskSetupFinished[taskId].set(true);
    vTaskDelay(pdMS_TO_TICKS(10));

    //bool aux=false;

    while(1) {

        if(tasksManager[taskId].periodEnded()) {
            
            lidar.update();
            lidarDist = lidar.getDistance() / 10; // cm

            gLidarDist.set(lidarDist);
            async.setLocalValue(0, lidarDist); // TODO: Decide if asyncMsg or SharedVar
            
        } else {
            async.update();
            msgQueue_t lastMessage = tasksManager[taskId].getLastQueueMsg();

            if (lastMessage.msg == AsyncMsg::WRITE_SERVO) {
                // Write to PWM lastMessage.index the value lastMessage.value

                calculatedValuePWM = map(lastMessage.value, 0, 180, SERVOMIN, SERVOMAX);
                calculatedValuePWM = constrain(calculatedValuePWM, SERVOMIN, SERVOMAX);
                
                Serial.print("I2C Servo: ");
                Serial.print(lastMessage.index);
                Serial.print(" ");
                Serial.print(lastMessage.value);
                Serial.print(" ");
                Serial.println(calculatedValuePWM);
                
                pwm.setPWM(lastMessage.index, 0, calculatedValuePWM);

            }
        }
    }
}

