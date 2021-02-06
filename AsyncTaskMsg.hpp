#pragma once
#include <array>
#include "AsyncTask.hpp"
#include "configGlobalVars.hpp"

class AsyncTaskMsg {
private:
    std::array<int, 16> _asyncValues{}; // Async messages comunication array. Initialized to 0
    std::array<int, 16> _asyncValuesRead{}; // Async messages comunication array. Initialized to 0
    AsyncTask* _ptrAsyncTask;
    int32_t _taskId;
    taskNames_t _myTaskName;

public:
    AsyncTaskMsg(taskNames_t myTaskName);
    void update();
    int getLocalValue(int index);
    void setLocalValue(int index, int value);

    // TODO: Move it to AsyncTask ????????????????????????????
    
    void readRemoteValue(taskNames_t taskName, int index);
    int getLastRemoteValue(int index);
    void writeRemoteValue(taskNames_t taskName, int index, int value);

    ~AsyncTaskMsg();
};
