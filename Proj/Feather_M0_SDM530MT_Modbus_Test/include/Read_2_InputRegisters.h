#pragma once

#ifndef _READ_2_INPUT_REGISTERS_H_
#define _READ_2_INPUT_REGISTERS_H_
#endif

#include <inttypes.h>
#include <ModbusRtu.h>

typedef struct
{
    bool Validity;
    int16_t ErrorCode;
    uint16_t HighReg;         /* higher register */
    uint16_t LowReg;          /* lower register */   
}
regsReturnStruct;

enum ERR_LOC_LIST : int16_t
{
    ERR_LOC_SUCCESS                     = int16_t(Modbus::Error::SUCCESS),
    ERR_LOC_NOT_HOST                    = int16_t(Modbus::Error::NOT_HOST),
    ERR_LOC_POLLING                     = int16_t(Modbus::Error::POLLING),
    ERR_LOC_BUFF_OVERFLOW               = int16_t(Modbus::Error::BUFF_OVERFLOW),
    ERR_LOC_BAD_CRC                     = int16_t(Modbus::Error::BAD_CRC),
    ERR_LOC_EXCEPTION                   = int16_t(Modbus::Error::EXCEPTION),
    ERR_LOC_NO_REPLY                    = int16_t(Modbus::Error::NO_REPLY),
    ERR_LOC_RUNT_PACKET                 = int16_t(Modbus::Error::RUNT_PACKET),
    ERR_LOC_ILLEGAL_DEVICE_ADDRESS      = int16_t(Modbus::Error::ILLEGAL_DEVICE_ADDRESS),
    ERR_LOC_NOT_RELEASED                = -9,
    ERR_LOC_BEGIN_FUNCTION_NOT_EXECUTED = -10
};

class Read_2_InputRegisters
    { 
    public:
        // constructor
        Read_2_InputRegisters(modbus_t pDatagram, uint16_t pInputRegAdress);

        void begin(Modbus pModbus, uint32_t pInitialReleaseTimespan_Ms);
       
        regsReturnStruct get_2_InputRegisters(uint8_t pSlaveAddress, uint32_t pReleaseTimespan);

        bool isReleased();

        void setReleaseTimespan(uint32_t pReleaseTimespan);

        uint16_t getRegisterAddress();

    private:
       uint16_t inputRegAddress;
       regsReturnStruct regsRetStruct;

       modbus_t datagramLocal;
       uint16_t au16dataLocal[16];

       volatile uint32_t releaseTime_Ms;
       volatile uint32_t releaseTimespan_Ms = 1000;
       Modbus host;
       bool ModbusHostIsInitialized = false;
       volatile int16_t errorCode;
       volatile uint32_t waitTime_All_Ms;
       volatile uint32_t waitTime_Between_Ms;
    };