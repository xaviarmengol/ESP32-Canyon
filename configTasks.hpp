#pragma once
#include <array>

typedef enum AsyncMsg {NO_MESSAGE, READ_VAR, WRITE_VAR, PERIOD_TIME_OUT, RESET_CALIB, TOTAL_MSG} asyncMsg_t;

// All tasks above TOTAL_TASK are internal tasks
typedef enum TaskNames {POSITION, WIFI, DEBUG, TOTAL_TASK, _TIMER} taskNames_t;

