#include "AsyncTaskMsg.hpp"

AsyncTaskMsg::AsyncTaskMsg(taskNames_t myTaskName) {
    _myTaskName = myTaskName;
    _taskId = static_cast<int>(myTaskName);
}

void AsyncTaskMsg::update(){
    _ptrAsyncTask = &(tasksManager[_taskId]);

    taskNames_t taskFrom = _ptrAsyncTask->getMsgTaskFrom();
    asyncMsg_t msg = _ptrAsyncTask->getMsgName();
    int index = _ptrAsyncTask->getMsgIndex();
    int value = _ptrAsyncTask->getMsgValue();

    if (msg == AsyncMsg::WRITE_VAR) {
        _asyncValues.at(index) = value;

    } else if (msg == AsyncMsg::READ_VAR) {
        //Answer with specific message, and with the same index, and with the READ value
        tasksManager[(int)taskFrom].sendMessage(
            AsyncMsg::ANSWER_READ_VAR, (taskNames_t)_taskId, index, _asyncValues.at(index));

    } else if (msg == AsyncMsg::ANSWER_READ_VAR) {
        _asyncValuesRead.at(index) = value;
    }

}

int AsyncTaskMsg::getLocalValue(int index) {
    return(_asyncValues.at(index));
}

void AsyncTaskMsg::setLocalValue(int index, int value) {
    _asyncValues.at(index) = value;
}

int AsyncTaskMsg::getLastRemoteValue(int index) {
    return(_asyncValuesRead.at(index));
}

void AsyncTaskMsg::readRemoteValue(taskNames_t taskName, int index) {
    tasksManager[(int)taskName].sendMessage(AsyncMsg::READ_VAR, _myTaskName, index, 0);
}

void AsyncTaskMsg::writeRemoteValue(taskNames_t taskName, int index, int value) {
    tasksManager[(int)taskName].sendMessage(AsyncMsg::WRITE_VAR, _myTaskName, index, value);
}

AsyncTaskMsg::~AsyncTaskMsg() {
}
