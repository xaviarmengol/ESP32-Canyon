#include "AsyncTask.hpp"

AsyncTask::AsyncTask() {}

AsyncTask::AsyncTask(taskNames_t taskName, unsigned long waitMs) {
    _taskName = taskName;
    _periodMs = waitMs;
    _queue = xQueueCreate( MAX_QUEUE , sizeof(asyncMsg_t));
    if (_queue == 0 ) Serial.println("Failed to create Queue");
    Serial.println((int)_queue);
    Serial.println("Queue created!");
}

//AsyncTask::AsyncTask(int taskNum, unsigned long waitMs) {
//    AsyncTask(static_cast<taskNames_t>(taskNum), waitMs);
//}

AsyncTask::~AsyncTask() {
}

TaskHandle_t* AsyncTask::getTaskHandler() {
    return(&_taskHandler);
}

void AsyncTask::setAsyncMsgCallBack(std::function<void(asyncMsg_t)> fManageAsyncMsg) {
    if (fManageAsyncMsg) _fManageAsyncMsg = fManageAsyncMsg;
}

bool AsyncTask::sendMessage(asyncMsg_t* ptrMsg) {
    Serial.print("Sending message: ");
    Serial.println((int)*ptrMsg);
    return(xQueueSend(_queue, ptrMsg, 0) == pdTRUE);
}

bool AsyncTask::sendMessage(asyncMsg_t msg) {
    _asyncMsgToSend = msg;

    Serial.print("Sending message: ");
    Serial.println((int)_asyncMsgToSend);
    return(xQueueSend(_queue, &_asyncMsgToSend, 0) == pdTRUE);
}

bool AsyncTask::hasPeriodElapsed(std::function<void(asyncMsg_t)> fManageAsyncMsg) {


    setAsyncMsgCallBack(fManageAsyncMsg);

    // TODO: Review if necessary to delay some ticks

    // Check if there is a new message in the queue
    //Serial.println("Before Queue Receive");
    if (_queue == 0 ) Serial.println("Queue NOT well created");
    //Serial.println((int)_queue);
    _newValue = (xQueueReceive(_queue, &_asyncMsgReceived, pdMS_TO_TICKS(DEFAULT_MIN_WAIT_TASK_MS)) == pdTRUE);
    //Serial.println("After Queue Receive");

    // if new message, and we have an informed call back, process it.
    if (_newValue && _fManageAsyncMsg) {
        Serial.print("Message received: ");
        Serial.println((int)_asyncMsgReceived);

        _startAsyncMsgMicros = micros();
        _fManageAsyncMsg(_asyncMsgReceived);
        _periodAsyncMsgMicros += micros() - _startAsyncMsgMicros;
    }

    // If the period algorithm has been executed
    if (_periodElapsed) {
        _periodBusyMicros = micros() - _startBusyMicros;
        _periodElapsed = false;
        _periodBusyPercentage = (_periodBusyMicros + _periodAsyncMsgMicros)  / (_periodMs * 10); // (100 / 1000) 
    }

    // The period has elapsed
    if ((micros() - _lastMicros) > (_periodMs * 1000)) {
        _lastMicros = micros();
        _periodElapsed = true;
        _startBusyMicros = micros();
        _periodAsyncMsgMicros = 0; // Re-start async message managament accumulative time counter
    }


    return(_periodElapsed);
    
}


asyncMsg_t AsyncTask::getLastValidAsyncMsg() {
    return(_asyncMsgReceived);
}


void AsyncTask::modifyWaitMs(unsigned long newMs) {
    _periodMs = newMs;
}

int AsyncTask::getBusyPercentage() {
    return(_periodBusyPercentage);
}