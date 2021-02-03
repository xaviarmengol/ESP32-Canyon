# ESP32-Canyon
Automated Playmobil canyon, based on ESP32 and Blynk

# FreeRTOS project template
This project is also used as a personal template to start other FreeRTOS projects.

## Project template structure:

### Config files
Config files (ex configPinOut.hpp / .cpp) are files where the project configuration is defined

* configGlovalVars: Files where define global variables and objects. In the .CPP we define it and in the .HPP we declare it using "extern" keyword to be used in any file. (https://stackoverflow.com/questions/1433204/how-do-i-use-extern-to-share-variables-between-source-files)

* configPinOut: Files where we name and assign the used Pinout (.HPP), and where we declare how are we going to use it (.CPP).

* configTasks: File where we declare the names of every task used, and every message interchanged between tasks. Every enum has a TOTAL_* member, that defines the number of members that the list has. Any declaration beyond this label, in internal.

* configBlynk: File here all the Blynk macros are declared (Only input data from Blynk to the Device).


### Main.ccp

File where we mainly initialize the global variables, and launch the tasks.
* The most important decision that we should take in this file is in which processor and with the priority every task should be launched


### Special Tasks:

Special tasks are used to deal with hardware in a thread safe way. Any task can interact with messages with the hardware tasks, and the hardware tasks deal with the messages asynchronously.

* tskGPIO: Task in charge of interact with IOs, Servos, etc..
* tskI2C: Task in charge of interacting with I2C bus
* tskWIFI: Task in charge of interacting with Wifi
* tskPeriodicTimer: Very special task that is used to send a periodic timeout to every task, at a predefined frequency. It is used as a "clock" of the cyclic task of every task.


### SharedVar class

This is an important class that allows to publish and consume objects, globally, in a thread safe mode (even in a multicore environment). Is designed to be used by single (or multiple) real time producers and consumed by a single (or multiple) real time consumers. 

* It has been templarized, and can be used with any type or object.
* It only keeps the most recent written value, but it is never deleted.
* It is internally designed with a Queue of 1 unit of length where the writer overwrite the value, and the consumer seek it.

### AsyncTask class

Manage the global object responsible of keeping all the tasks information. It is used to:

* Keep the task blocked until an event comes. The event can be a "Periodic Event" or a "Async Event".
* Send messages to other tasks

### AsyncTaskMsg class

Used to encapsulate the basic messaging protocol, with an "index" (int32_t) and a "value" (int32_t).

* READ_VAR: Used to read a remote value. In other words, ask another task to send back the value in the position "index".
* ANSWER_READ_VAR: Used to send back to the requested value that is in the "index".
* WRITE_VAR: Used to directly write the value "value" in the position "index" of the destination task.

The object automatically declares two internal arrays:
* one with the public values that can be read/write from outside the task.
* a second one, with the values that have been read using READ_VAR.

### Control Files (Facades)

Control files (ex: CtrlServoMsg.hpp / .cpp) are simply Facades used to uncouple our code from external library used. Any external library should be managed by a Ctrl class, and only the Ctrl class should be used in the code. (https://en.wikipedia.org/wiki/Facade_pattern)









