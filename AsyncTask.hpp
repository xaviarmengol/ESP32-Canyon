#pragma once

////////////////////////

/// Async messages class

#include <Arduino.h>
#include <configTasks.hpp>

#include <functional> // Call back

#define DEBUG_MODE false

constexpr int MAX_QUEUE = 10;
constexpr unsigned long DEFAULT_PERIOD_MS = 10;
constexpr int DEFAULT_MIN_WAIT_TASK_MS = 2;

// Type that define the message sent in the Freertos Queues
typedef struct MessageQueue {
    asyncMsg_t msg; // Msg code
    taskNames_t taskFrom; // Task where the msg come from
    int32_t index; // Variable index
    int32_t value; // Value

    /*
    MessageQueue& operator = (const MessageQueue &t) { 
        msg = t.msg;
        taskFrom = t.taskFrom;
        index = t.index;
        value = t.value;
    }

    MessageQueue(const MessageQueue &t)  {  
        msg = t.msg;
        taskFrom = t.taskFrom;
        index = t.index;
        value = t.value;
    }  
    */
   
} msgQueue_t;

class AsyncTask {
public:
    AsyncTask();
    AsyncTask(taskNames_t taskName, unsigned long waitMs = 10);
    ~AsyncTask();

    bool periodEnded();
    void modifyWaitMs(unsigned long newMs);
    unsigned long getWaitMs();
    int getBusyPercentage();

    msgQueue_t getLastQueueMsg();

    asyncMsg_t getMsgName();
    taskNames_t getMsgTaskFrom();
    int32_t getMsgIndex();
    int32_t getMsgValue();

    void printMessage(msgQueue_t msgQueue);

    // Send Message msg from taskFrom
    bool sendMessage(asyncMsg_t msg, taskNames_t taskFrom, int32_t index = -1, int32_t value = -1);

    TaskHandle_t* getTaskHandler();

private:

    TaskHandle_t _taskHandler = new TaskHandle_t(); // Created in HEAP

    QueueHandle_t _queue;
    taskNames_t _taskName;
    unsigned long _periodMs = 10;

    asyncMsg_t _asyncMsgReceived = AsyncMsg::NO_MESSAGE;
    //asyncMsg_t _asyncMsgToSend = AsyncMsg::NO_MESSAGE;

    msgQueue_t _msgQueueReceived = {AsyncMsg::NO_MESSAGE, TaskNames::DEBUG, -1, -1};
    //msgQueue_t _msgQueueToSend = {AsyncMsg::NO_MESSAGE, TaskNames::DEBUG, -1, -1};

    taskNames_t _taskFromMsgReceived;
    int32_t _indexMsgReceived;
    int32_t _valueMsgReceived;
    
    bool _periodElapsed = true;

    unsigned long _startBusyMicros = 0;
    unsigned long _periodBusyMicros = 0;
    
    int _periodBusyPercentage;

    unsigned long _startAsyncMsgMicros = 0;
    unsigned long _periodAsyncMsgMicros = 0;
    unsigned long _totalPeriodAsyncMicros = 0;
};
