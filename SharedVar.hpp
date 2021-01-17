#pragma once

#include <Arduino.h>

// TODO: Is it necessary?
constexpr TickType_t SHARED_DELAY = 0; // Small delay in case we do a set and get very close one to another

template <class T>
class SharedVar {
    public:
        SharedVar() {
            // Only 1 position because we are interested in real time value
            _queue = xQueueCreate( 1 , sizeof(T));
            _allocated = (_queue != NULL);

            if(!_allocated) Serial.println("Variable can not be allocated. Check memory allocation.");
            else {
                set(T());
            }
                
        };

        void set (T value) {
            if (_allocated) {
                //xQueueOverwrite(_queue, &value);
                xQueueGenericSend(_queue, &value, SHARED_DELAY, queueOVERWRITE);
            } else {
                Serial.println("Variable not allocated in initialitzation. Check memory allocation.");
            }
        }

        T get () {
            int bufferWithValue;
            T value;
            if (_allocated) {
                
                bufferWithValue = (xQueuePeek(_queue, &value, SHARED_DELAY) == pdTRUE);

                if (bufferWithValue) {
                    return(value);
                }
                else return(T());

            } else {
                
                Serial.println("Variable not allocated in initialitzation. Check memory allocation.");
                return(T());
            }
        }

        void print(String name) {
            Serial.print(" | ");
            Serial.print(name);
            Serial.print(" : ");
            Serial.print(get());
        }

    private:
        QueueHandle_t _queue;
        bool _allocated=false;
};