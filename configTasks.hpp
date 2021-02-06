#pragma once
#include <array>

typedef enum AsyncMsg {NO_MESSAGE, READ_VAR, ANSWER_READ_VAR, WRITE_VAR, PERIOD_TIME_OUT, RESET_CALIB, WRITE_SERVO, WRITE_IO, READ_IO, TOTAL_MSG} asyncMsg_t;

// All tasks above TOTAL_TASK are internal tasks
typedef enum TaskNames {POSITION, WIFI, I2C, HARD_IO, DEBUG, TOTAL_TASK, _TIMER, _GENERIC} taskNames_t;

