#include "AsyncTask.hpp"

/// Crear una interrupció periòdica que envii un missatge de "Periode" a cada tasca
/// D'aquesta manera la tasca simplement es un receptor de missatges
///https://techtutorialsx.com/2017/10/07/esp32-arduino-timer-interrupts/#:~:text=The%20ESP32%20has%20two%20timer,the%20timer%20counter%20%5B2%5D.


AsyncTask::AsyncTask() {}

AsyncTask::AsyncTask(taskNames_t taskName, unsigned long waitMs) {
    _taskName = taskName;
    _periodMs = waitMs;
    _queue = xQueueCreate( MAX_QUEUE , sizeof(msgQueue_t));
    if (_queue == 0 ) {
        Serial.println("Failed to create Queue");
    } else {
        Serial.print("Queue created: ");
        Serial.println((int)_queue);
    }
}

AsyncTask::~AsyncTask() {
}

TaskHandle_t* AsyncTask::getTaskHandler() {
    return(&_taskHandler);
}


bool AsyncTask::sendMessage(asyncMsg_t msg, taskNames_t taskFrom, int32_t index, int32_t value) {

    bool returnValue=false;

    _msgQueueToSend = {msg, taskFrom, index, value};
    returnValue = (xQueueSend(_queue, &_msgQueueToSend, 0) == pdTRUE);

    if (taskFrom != TaskNames::_TIMER) {
        Serial.print("Sending message: ");
        if (!returnValue) Serial.print("ERROR "); else Serial.print("OK ");
        printMessage(_msgQueueToSend);
    }
    

    return(returnValue);
}

bool AsyncTask::hasPeriodElapsedWithExternalTimer() {
    if (_queue == 0 ) {
        Serial.println("Queue NOT well created");
    } 

    // Last iteration was due to the end of the period
    if (_periodElapsed) {
        _periodBusyMicros = micros() - _startBusyMicros;
    }

    // Last iteration was due to a new value coming from the Queue
    if (!_periodElapsed) {
        _periodAsyncMsgMicros = micros() - _startAsyncMsgMicros;
        _totalPeriodAsyncMicros += _periodAsyncMsgMicros;
    }

    // Block until Message
    xQueueReceive(_queue, &_msgQueueReceived, portMAX_DELAY);

    if (_msgQueueReceived.taskFrom != TaskNames::_TIMER) {
        Serial.print("Receiving message: ");
        printMessage(_msgQueueReceived);
    }
    
    _periodElapsed = (_msgQueueReceived.msg == AsyncMsg::PERIOD_TIME_OUT);

    // New message received
    if (!_periodElapsed) {
        _startAsyncMsgMicros = micros();

        _asyncMsgReceived = _msgQueueReceived.msg;
        _taskFromMsgReceived = _msgQueueReceived.taskFrom;
        _indexMsgReceived = _msgQueueReceived.index;
        _valueMsgReceived = _msgQueueReceived.value;

    // End of period. Starting again the cyclic execution.
    } else {
        _periodBusyPercentage = static_cast<int>((_periodBusyMicros + _totalPeriodAsyncMicros)  / (_periodMs * 10)); // (100 / 1000)
        _startBusyMicros = micros();
        _totalPeriodAsyncMicros = 0; // Reset the totals
    }

    return(_periodElapsed);
}


asyncMsg_t AsyncTask::getLastValidAsyncMsg() {
    return(_asyncMsgReceived);
}

void AsyncTask::modifyWaitMs(unsigned long newMs) {
    _periodMs = newMs;
}

unsigned long AsyncTask::getWaitMs() {
    return(_periodMs);
}

int AsyncTask::getBusyPercentage() {
    return(_periodBusyPercentage);
}

void AsyncTask::printMessage(msgQueue_t msgQueue) {
    Serial.print("To: ");
    Serial.print((int)_taskName);
    Serial.print(" From: ");
    Serial.print((int)msgQueue.taskFrom);
    Serial.print(" Msg: ");
    Serial.print((int)msgQueue.msg);
    Serial.print(" Index: ");
    Serial.print(msgQueue.index);
    Serial.print(" Value: ");
    Serial.print(msgQueue.value);
    Serial.print(" ms: ");
    Serial.println(millis());
}