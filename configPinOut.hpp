#pragma once

// Declarations

void pinModeConfig ();

// https://randomnerdtutorials.com/esp32-pinout-reference-gpios/

// Inputs: all except 6 to 11 that are connected to SPI Flash
// From 34 to 39 can not generate PWM
// 36 29 34 35 Input only

//Servo availible on: 2,4,5,12-19,21-23,25-27,32-33

// 24:62:AB:FF:35:50 -> No funciona pin 13

#define PIN_SERVO_ELEV 14
#define PIN_SERVO_ROT 12
#define PIN_SERVO_FIRE 27

// Radio control

#define RADIO_DIR 2
#define RADIO_VEL 4

// I2C

#define I2C_SDA 21
#define I2C_SCL 22

// IMU

#define INTERRUPT_PIN 15

// PIN DEBUG

#define DEBUG_PIN 23



