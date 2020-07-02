#ifndef COMSERIAL_H
#define COMSERIAL_H

#include "SerialPort.h"
#include "iostream"
#include "vector"

#define STX1 '\x0AA'
#define ETX1 '\x0FF'

//codes
//I, for start J  for change in the value of RPM, T  for time and , P for stop
#define START 'I'
#define RPM   'J'
#define TIME  'T'
#define STOP  'P'


enum rxStates
{
    Header = 0,
    DataState,
    Tailer

};

using namespace std;


class ComSerial
{
public:
    ComSerial();
    ~ComSerial();
    bool initSerial();
    void transmitData();
    bool receiveData();
    bool receiveData(uint8_t rx);

    void extractData(vector<uint8_t>data);

private:
    SerialPort *port;
    vector <u_int8_t> rxbuffer;
    u_int8_t recxState = Header;

    const u_char STX = 0xAA;
    const u_char ETX = 0xFF;

//    const uint8_t STX = '\x002';
//    const uint8_t ETX = '\x003';

public:
    vector <u_int8_t> txBuffer;
    vector <uint8_t> dataBuffer;


    struct DataMessage
    {
    uint32_t duration;
    uint16_t rpm;
    uint16_t current;
    uint16_t voltage;
    uint16_t temp;
    time_t ref_t;
    int16_t reference;
    uint32_t sumatoria;
    //uint8_t sumatoria;

    char code;

    DataMessage()
    {
        duration = 0;
        rpm = 0;
        current = 0;
        voltage = 0;
        temp = 0;
        ref_t = 0;
        reference = 0;
        sumatoria = 0;
        code = 0;
    }

    };

    DataMessage txSendStruct;
    DataMessage rxStruct;
};

#endif // COMSERIAL_H
