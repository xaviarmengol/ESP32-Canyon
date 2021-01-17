#include <Wire.h>
#include <Arduino.h>
#include "CtrlTOF10120.hpp"


TOF10120::TOF10120 () {

}


void TOF10120::_SensorRead(unsigned char addr,unsigned char* datbuf,unsigned char cnt) {

    // Read PDF

    if (_sendingState) {

        // step 1: instruct sensor to read echoes
        // the address specified in the datasheet is 164 (0xa4)
        // but i2c adressing uses the high 7 bits so it's 82
        Wire.beginTransmission(82); // transmit to device #82 (0x52)
        Wire.write(byte(addr));      // sets distance data address (addr)
        Wire.endTransmission();      // stop transmitting

        _timeStep1 = millis();
        _sendingState = false;
        // step 2: wait for readings to happen
        //delay(1);
    }           

    // Delay 1ms // datasheet suggests at least 30uS
    if (!_sendingState && (millis() - _timeStep1 >= 1)) {

        // step 3: request reading from sensor
        Wire.requestFrom(82, cnt);    // request cnt bytes from slave device #82 (0x52)

        // step 5: receive reading from sensor
        if (cnt <= Wire.available()) { // if two bytes were received
            *datbuf++ = Wire.read();  // receive high byte (overwrites previous reading)
            *datbuf++ = Wire.read(); // receive low byte as lower 8 bits
        }
        _sendingState = true;
    }
}

void TOF10120::update(){

    _SensorRead(0x00,_i2c_rx_buf,2);
    _lenth_val=_i2c_rx_buf[0];
    _lenth_val=_lenth_val<<8;
    _lenth_val|=_i2c_rx_buf[1];

}

int TOF10120::getDistance(){
    return (_lenth_val);
}







