#ifndef TOF10120_HPP_INCLUDED
#define TOF10120_HPP_INCLUDED

#include <Wire.h>

class TOF10120 {
    private:

        unsigned short _lenth_val = 0;
        unsigned char _i2c_rx_buf[16];
        unsigned char _dirsend_flag=0;

        unsigned long _timeStep1=0;
        bool _sendingState=true;

        void _SensorRead(unsigned char addr,unsigned char* datbuf,unsigned char cnt);


    public:
        TOF10120();
        void update();
        int getDistance();
};

#endif