#include "ComSerial.h"

#include "string.h"
#include "math.h"
#include <numeric>
ComSerial::ComSerial()
{
    port = new SerialPort("/dev/ttyS0");

}

ComSerial::~ComSerial()
{
    delete port;
}

bool ComSerial::initSerial()
{
    char buffer[40];
    char sp_name[10] = "/dev/";
    strcat(sp_name,"portname");
    sprintf(buffer,"sudo chown -R $USERNAME /dev/ttyS0");
    system(buffer);

    //    for (char c: buffer) {
    //        std::cout << c << " ";
    //    }

    port->Open(SerialPort::BAUD_9600,
               SerialPort::CHAR_SIZE_8,
               SerialPort::PARITY_ODD,
               SerialPort::STOP_BITS_1,
               SerialPort::FLOW_CONTROL_NONE);


    if(port->IsOpen())
        return true;
    else
        return false;

}

void ComSerial::transmitData()
{

//    uint32_t duration;
//    uint16_t rpm;
//    uint16_t current;
//    uint16_t voltage;
//    uint16_t temp;
//    time_t ref_t;
//    int16_t reference;


    //pushing all data bytes in a vector for sending
    txBuffer.push_back(STX);

    txBuffer.push_back(txSendStruct.duration);
    txBuffer.push_back(txSendStruct.duration >> 8);
    txBuffer.push_back(txSendStruct.duration >> 16);
    txBuffer.push_back(txSendStruct.duration >> 24);

    txBuffer.push_back(txSendStruct.rpm);
    txBuffer.push_back(txSendStruct.rpm >> 8);

    txBuffer.push_back(txSendStruct.current);
    txBuffer.push_back(txSendStruct.current >> 8);

    txBuffer.push_back(txSendStruct.voltage);
    txBuffer.push_back(txSendStruct.voltage >> 8);

    txBuffer.push_back(txSendStruct.temp);
    txBuffer.push_back(txSendStruct.temp >> 8);


    txBuffer.push_back(txSendStruct.ref_t);
    txBuffer.push_back(txSendStruct.ref_t >> 8);
    txBuffer.push_back(txSendStruct.ref_t >> 16);
    txBuffer.push_back(txSendStruct.ref_t >> 24);


    txBuffer.push_back(txSendStruct.reference);
    txBuffer.push_back(txSendStruct.reference >> 8);


    //for four byte sumatoria -- calculate sumotoria
    txSendStruct.sumatoria = txSendStruct.duration + txSendStruct.rpm + txSendStruct.current +
            txSendStruct.voltage + txSendStruct.temp  + txSendStruct.reference;


    txBuffer.push_back(txSendStruct.sumatoria);
    txBuffer.push_back(txSendStruct.sumatoria >> 8);
    txBuffer.push_back(txSendStruct.sumatoria >> 16);
    txBuffer.push_back(txSendStruct.sumatoria >> 24);

    txBuffer.push_back(txSendStruct.code);

    txBuffer.push_back(ETX);

//    for (auto c:txBuffer)
//        cout << (int)c << " ";
//    cout << endl;

    port->Write(txBuffer);
    txBuffer.clear();

}

bool ComSerial::receiveData()
{
    u_char rxbyte;
    bool ret_val = false;

    rxbyte  = port->ReadByte(0);

    switch (recxState)
    {
    case Header:

        if(rxbyte == STX)
        {
            recxState = DataState;
            rxbuffer.push_back(rxbyte);
        }
        else
        {
            recxState = Header;
            rxbuffer.clear();
        }
        break;

    case DataState:

        if(rxbyte == ETX)
        {

            rxbuffer.push_back(rxbyte);

            //calcuting checksum for received data
            uint32_t chksum = 0;

            for (int i=1 ;i<13; i++) {
             chksum = chksum + rxbuffer[i];
            }
            chksum = chksum + rxbuffer[17] + rxbuffer[18];

            //checksum that is sent with received data
            uint calc_chksum = (rxbuffer[19]) + (rxbuffer[20] << 8) + (rxbuffer[21] << 16) + (rxbuffer[22] << 24);

            //means data is correct
            if((rxbuffer[0] == STX) && (calc_chksum == chksum))
            {
                recxState = Header;
                dataBuffer = rxbuffer;
                rxbuffer.clear();
                ret_val = true;
                return ret_val;
            }
        }
        else
        {
            rxbuffer.push_back(rxbyte);
        }
        break;

    default:
        recxState = Header;
        rxbuffer.clear();

    }
}

bool ComSerial::receiveData(uint8_t rx)
{
    u_char rxbyte = rx;
    bool ret_val = false;

    switch (recxState)
    {
    case Header:

        if(rxbyte == STX)
        {
            recxState = DataState;
            rxbuffer.push_back(rxbyte);
        }
        else
        {
            recxState = Header;
            rxbuffer.clear();
        }
        break;

    case DataState:

        if(rxbyte == ETX)
        {

            rxbuffer.push_back(rxbyte);

            //calcuting checksum for received data
            uint32_t chksum = 0;

            for (int i=1 ;i<13; i++) {
             chksum = chksum + rxbuffer[i];
            }
            chksum = chksum + rxbuffer[17] + rxbuffer[18];

            //checksum that is sent with received data
            uint calc_chksum = (rxbuffer[19]) + (rxbuffer[20] << 8) + (rxbuffer[21] << 16) + (rxbuffer[22] << 24);

            //means data is correct
            if((rxbuffer[0] == STX) && (calc_chksum == chksum))
            {
                recxState = Header;
                dataBuffer = rxbuffer;
                rxbuffer.clear();
                ret_val = true;
                return ret_val;
            }           

        }
        else
        {
            rxbuffer.push_back(rxbyte);
        }
        break;

    default:
        recxState = Header;
        rxbuffer.clear();

    }
}

void ComSerial::extractData(vector<uint8_t>data)
{

    int byteNo = 1;
    rxStruct.duration = data[byteNo++] | data[byteNo++] << 8 | data[byteNo++] << 16 | data[byteNo++] << 24;
    rxStruct.rpm = data[byteNo++]| data[byteNo++]<< 8;

    rxStruct.current = data[byteNo++] | data[byteNo++] << 8;
    rxStruct.voltage = data[byteNo++] | data[byteNo++] << 8;

    rxStruct.temp = data[byteNo++]| data[byteNo++] << 8;

    rxStruct.ref_t = data[byteNo++]| data[byteNo++] << 8 | data[byteNo++] << 16 | data[byteNo++] << 24;

    rxStruct.reference = data[byteNo++]| data[byteNo++] << 8;


}
