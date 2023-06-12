#include "SerialDriver.h"
SerialDirver::SerialDirver()
    :_type(SerialType)
    ,_port(new CSerialPort()) 
    ,_listener(new SerialListener(_port,this))
{
    if((_port!= nullptr)&&(_listener!= nullptr))
    {
        _port->connectReadEvent(_listener);
    }
}
/***********************************************************/
SerialDirver::~SerialDirver()
{
    delete _port;
    delete _listener;
    _port =nullptr;
    _listener = nullptr;
}
/***********************************************************/
void SerialDirver::init(const char *portName,
                        int baudRate,
                        Parity parity,
                        DataBits dataBits,
                        StopBits stopbits,
                        FlowControl flowControl ,
                        unsigned int readBufferSize)
{
   _port->init(portName,baudRate,parity,dataBits,stopbits,flowControl,readBufferSize);
}
/***********************************************************/
SerialListener::SerialListener(CSerialPort *port,SerialDirver* driver)
    :_port(port)
    ,_driver(driver)
{

};
/***********************************************************/
void SerialListener::onReadEvent(const char* portName, unsigned int readBufferLen) 
{
    if (_driver!=nullptr) 
    {
        _driver->onReadEvent(portName,readBufferLen);
    }
}
/***********************************************************/
