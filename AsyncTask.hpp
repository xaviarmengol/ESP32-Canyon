#pragma once

////////////////////////

/// Async messages class

#include <Arduino.h>
#include <configTasks.hpp>

#include <functional> // Call back

constexpr int MAX_QUEUE = 10;
constexpr unsigned long DEFAULT_PERIOD_MS = 10;
constexpr int DEFAULT_MIN_WAIT_TASK_MS = 2;


class AsyncTask {
public:
    AsyncTask();
    AsyncTask(taskNames_t taskName, unsigned long waitMs = 10);
    //AsyncTask(int taskNum, unsigned long waitMs = DEFAULT_PERIOD_MS);
    ~AsyncTask();

    bool hasPeriodElapsed(std::function<void(asyncMsg_t)> manageAsyncMsg = nullptr);
    asyncMsg_t getLastValidAsyncMsg();
    void modifyWaitMs(unsigned long newMs);
    int getBusyPercentage();
    bool sendMessage(asyncMsg_t* ptrMsg);
    bool sendMessage(asyncMsg_t msg);

    TaskHandle_t* getTaskHandler();
    void setAsyncMsgCallBack(std::function<void(asyncMsg_t)> manageAsyncMsg);


private:

    TaskHandle_t _taskHandler;

    QueueHandle_t _queue;
    taskNames_t _taskName;
    unsigned long _periodMs = 10;
    asyncMsg_t _asyncMsgReceived = AsyncMsg::NO_MESSAGE;
    asyncMsg_t _asyncMsgToSend = AsyncMsg::NO_MESSAGE;
    bool _newValue;

    unsigned long _startBusyMicros;
    unsigned long _periodBusyMicros;
    int _periodBusyPercentage;

    unsigned long _startAsyncMsgMicros;
    unsigned long _periodAsyncMsgMicros;

    bool _periodElapsed = false;
    unsigned long _lastMicros = 0;


    std::function<void(asyncMsg_t)> _fManageAsyncMsg;
    
};
