#include "Read_2_InputRegisters.h"
#include <inttypes.h>
#include <ModbusRtu.h>


Read_2_InputRegisters::Read_2_InputRegisters(modbus_t pDatagram, uint16_t pInputRegAddress)
{
    inputRegAddress = pInputRegAddress;  
}

void Read_2_InputRegisters::begin(Modbus pHost, uint32_t pInitialReleaseTimespam)
{
    releaseTimespan_Ms = pInitialReleaseTimespam;
    releaseTime_Ms = millis() + releaseTimespan_Ms;   
    host = pHost;
    ModbusHostIsInitialized = true;
}

bool Read_2_InputRegisters::isReleased()
{
    return ((int32_t)(millis() - releaseTime_Ms) > 0);
}

uint16_t Read_2_InputRegisters::getRegisterAddress()
{
    return inputRegAddress;
}

void Read_2_InputRegisters::setReleaseTimespan(uint32_t pReleaseTimespan)
{
    releaseTimespan_Ms = pReleaseTimespan;
    releaseTime_Ms = millis() + releaseTimespan_Ms;
}

regsReturnStruct Read_2_InputRegisters::get_2_InputRegisters(uint8_t pSlaveAddress, uint32_t pReleaseTimespan, ForceState pForceState)
{
    regsRetStruct.HighReg = 0;
    regsRetStruct.LowReg = 0;
    regsRetStruct.Validity = false;
    regsRetStruct.ErrorCode = ERR_LOC_NOT_RELEASED;
    releaseTimespan_Ms = pReleaseTimespan;
        

    volatile uint16_t theCopy = inputRegAddress; 
    if ((long(millis() - releaseTime_Ms) > 0) || pForceState == ForceState::IgnoreReleaseTime)
    {       
        datagramLocal.u8id = pSlaveAddress; // device address 
        datagramLocal.u8fct = 4;
        datagramLocal.u16RegAdd = inputRegAddress;
        datagramLocal.u16CoilsNo = 2;       
        datagramLocal.au16reg = au16dataLocal;
        if (!ModbusHostIsInitialized)
        {
            regsRetStruct.ErrorCode = int(ERR_LOC_BEGIN_FUNCTION_NOT_EXECUTED);
            regsRetStruct.Validity = false;
            releaseTime_Ms = millis() + 1000;
            return regsRetStruct; 
        }

        host.setLastError(ERR_SUCCESS);
        host.query( datagramLocal ); // send query (only once)
        delay(10);

        volatile bool receivedAnswer = false;
        waitTime_Between_Ms = millis() + 1;
        while (!receivedAnswer)
        {
            while (long(millis() - waitTime_Between_Ms) < 0)
            {} 
            host.poll(); // check incoming messages
            if (host.getState() == COM_IDLE) 
            {         
                ERR_LIST lastError = host.getLastError();
                if (host.getLastError() != ERR_SUCCESS) 
                {                                    
                    regsRetStruct.ErrorCode = int(lastError);
                    regsRetStruct.Validity = false;
                    releaseTime_Ms = millis() + 1000;  // if failed, allow again after 1000 ms                
                } 
                else 
                {                        
                    regsRetStruct.HighReg = datagramLocal.au16reg[0];                  
                    regsRetStruct.LowReg = datagramLocal.au16reg[1];  
                    //Serial.println("Hallo");
                    regsRetStruct.ErrorCode = int(lastError);
                    regsRetStruct.Validity = true;
                    releaseTime_Ms = millis() + releaseTimespan_Ms;   // if succeeded, allow again after selected time                
                }
                receivedAnswer = true;
            }
            else
            {
                waitTime_Between_Ms =  millis() + 2;
            }                
        }                     
    }
    return regsRetStruct;          
}
