// Copyright: RoSchmi 2020 License: GNU Lesser General Public License v2.1
// This is a Modubus transmission example
// The code runs on a Feather M0 board, which serves as the Modbus host
// The program reads Input Registers Power and Input-Work from a Eastron SDM530-MT Smartmeter
// The Modbus libraries are from
// https://github.com/mcci-catena/Modbus-for-Arduino
//
// This Application uses the Class 'Read_2_InputRegisters' with two instances
// (readSummedCurrent) and (readImportWork) to make the main class more easy understandable
// A Class DataContainer is introduced for averaging of received Current values over time 

#include <Arduino.h>
#include <stdint.h>
#include "wiring_private.h" // pinPeripheral() function
#include "variant.h"
#include <ModbusRtu.h>
#include "Reform_uint16_2_float32.h"
#include "Read_2_InputRegisters.h"
#include "DataContainer.h"

/********* Values to be changed by User   ********/
const uint8_t SlaveAddress = 1;
const uint32_t InitialReleaseTimespan_Current_Ms = 1000;   // First 'Current' read after this time
const uint32_t InitialReleaseTimespan_Power_Ms = 1000;     // First 'Power' read after this time
const uint32_t InitialReleaseTimespan_Work_Ms = 1000;      // First 'Work' read after this time

const uint32_t FollowReleaseTimespan_Current_Ms = 5000;   // Following 'Current' reads after this time
const uint32_t FollowReleaseTimespan_Power_Ms = 5000;     // Following 'Power' reads after this time
const uint32_t FollowReleaseTimespan_Work_Ms = 5000;      // Following 'Work' reads after this time

/********* end of values to be changed by User ****/

// Some values with their Register Addresses that can be read from Smartmeter
const uint16_t Phase_1_Current_Address = 0x0006;    //Phase 1 current
const uint16_t Phase_2_Current_Address = 0x0008;    //Phase 2 current
const uint16_t Phase_3_Current_Address = 0x000A;    //Phase 3 current
const uint16_t Sum_of_Current_Address = 0x0030;     //Sum of line currents
const uint16_t Total_System_Power_Address = 0x0034; //Total System Power
const uint16_t Import_Work_Address = 0x0048;        //Import Wh since last reset
const uint16_t Export_Work_Address = 0x004A;        //Export Wh since last reset

// data array for modbus network sharing
uint16_t au16data[16];
uint8_t u8addr;

float NotValidValue = 999.9;
float ActCurrent = NotValidValue;

uint32_t ActImportWork = 0;
uint16_t ImpWorkHigh;
uint16_t ImpWorkLow;
SampleValues sampleValues;
bool isForcedReadInputWork = true;
//Call Constructor of DataContainer
// Parameters means: send at least every 10 min or if the deviation is more than 10% or more than 0.2 Amps
DataContainer dataContainer(10 * 60 * 1000, TriggerDeviation::PERCENT_10, TriggerDeviationAmps::AMPERE_02);

// not used in this example
#define kPowerOn        A3

static inline void powerOn(void)
{
        pinMode(kPowerOn, OUTPUT);
        digitalWrite(kPowerOn, HIGH);
}

/**
 *     This is a struct which contains a query to a device
 */
modbus_t datagram;

int16_2_float_function_result reform_uint16_2_float32(uint16_t u1, uint16_t u2);

regsReturnStruct get_2_InputRegisters(uint8_t pSlaveAddress);

regsReturnStruct regsRetStruct;

unsigned long u32wait;

// Serial2 pin and pad definitions (in Arduino files Variant.h & Variant.cpp)
#define PIN_SERIAL2_RX       (34ul)               // unsigned long (32 bit )Pin description number for PIO_SERCOM on D12 - PA19
#define PIN_SERIAL2_TX       (36ul)               // unsigned long (32 bit )Pin description number for PIO_SERCOM on D10 - PA18
#define PAD_SERIAL2_TX       (UART_TX_PAD_2)      // SERCOM pad 2
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_3)    // SERCOM pad 3

// Instantiate the Serial2 class if needed
// Uart Serial2(&sercom1, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX);

/**
 *  Modbus object declaration
 *  u8id : node id = 0 for host, = 1..247 for device
 *  u8serno : serial port (use 0 for Serial)
 *  u8txenpin : 0 for RS-232 and USB-FTDI 
 *               or any pin number > 1 for RS-485
 */
Modbus host(0, A4); // this is host and RS-232 or USB-FTDI

ModbusSerial<decltype(Serial1)> mySerial(&Serial1);
// ModbusSerial<decltype(Serial2)> mySerial(&Serial2);

// Calling constructors (the .begin function of each class must be called in Setup)
// Read_2_InputRegisters readTotalSystemPower(datagram, Total_System_Power_Address);
Read_2_InputRegisters readImportWork(datagram, Import_Work_Address);
Read_2_InputRegisters readSummedCurrent(datagram, Sum_of_Current_Address);

void setup() {
  // put your setup code here, to run once:
  powerOn();
  host.begin(&mySerial, 2400, SERIAL_8N1); // baud-rate at 2400
  host.setTimeOut( 1000 ); // if there is no answer in 1000 ms, roll over
  host.setTxEnableDelay(100);

  // readTotalSystemPower.begin(host, InitialReleaseTimespan_Ms);
  readImportWork.begin(host, InitialReleaseTimespan_Work_Ms);
  readSummedCurrent.begin(host, InitialReleaseTimespan_Current_Ms);
 
  u8addr = SlaveAddress;
  
  while (!Serial); // wait until serial console is open, remove if not tethered to computer
  Serial.begin(115200);
  Serial.println("Hallo"); 
}

void loop() 
{
  // put your main code here, to run repeatedly:
     
        if (regsRetStruct.ErrorCode == (int16_t)ERR_LOC_BEGIN_FUNCTION_NOT_EXECUTED)
        {
              Serial.println("ERROR: .begin function not executed in Setup");
        }  
        // Read 'Current' from the Smartmeter if its releaseTime has expired
        if (readSummedCurrent.isReleased())
        {
          regsRetStruct = readSummedCurrent.get_2_InputRegisters(u8addr, FollowReleaseTimespan_Current_Ms, ForceState::RespectReleaseTime);
          if (regsRetStruct.ErrorCode == (int16_t)ERR_SUCCESS)
          {
            //Calculate value as float and print out
            int16_2_float_function_result resultStruct = reform_uint16_2_float32(regsRetStruct.HighReg, regsRetStruct.LowReg);
            if (resultStruct.validity == true)
            {
              ActCurrent = resultStruct.value;
              dataContainer.SetNewValues(millis(), ActCurrent, ActImportWork);
              Serial.println("  " + String(resultStruct.value, 4) + "  Amps");             
            }
            else
            {
              ActCurrent = NotValidValue;
              Serial.println("Current value not valid "); 
            } 
          }
          else
          {
            ActCurrent = NotValidValue;
            Serial.print("Reading Power failed. Error Code: " );
            Serial.print((int16_t)regsRetStruct.ErrorCode);
            Serial.println("");
          }
          
        }
        // Read 'Work' from the Smartmeter if its releaseTime has expired
        if (readImportWork.isReleased() || isForcedReadInputWork)
        {
          ForceState forceState = isForcedReadInputWork ? IgnoreReleaseTime : RespectReleaseTime;
          
          isForcedReadInputWork = false;
          
          regsRetStruct = readImportWork.get_2_InputRegisters(u8addr, FollowReleaseTimespan_Work_Ms, forceState);
          if (regsRetStruct.ErrorCode == (int16_t)ERR_SUCCESS)
          {
            //Calculate value as float and print out
            ImpWorkLow = regsRetStruct.LowReg;
            ImpWorkHigh = regsRetStruct.HighReg;
            ActImportWork = (((uint32_t)regsRetStruct.HighReg) << 16) | regsRetStruct.LowReg;

            int16_2_float_function_result resultStruct = reform_uint16_2_float32(regsRetStruct.HighReg, regsRetStruct.LowReg);
            if (resultStruct.validity == true)
            {

              Serial.println("  " + String(resultStruct.value, 4) + "  KWh");                     
            }
            else
            {
              Serial.println("Work value not valid "); 
            } 
          }
           else
          {
            ActImportWork = 0xffffffff;
            Serial.print("Reading Work failed. Error Code: ");
            Serial.print((int16_t)regsRetStruct.ErrorCode);
            Serial.println("");           
          }
        }
        
      
        if (dataContainer.hasToBeSent())
        {
          sampleValues = dataContainer.getSampleValuesAndReset();
          Serial.print("Got valus from DataContainer: Current: ");
          Serial.print(sampleValues.AverageCurrent);
          Serial.print(" Amps ");        
          int16_2_float_function_result resultStruct = reform_uint16_2_float32(ImpWorkHigh, ImpWorkLow);
          Serial.print(" " + String(resultStruct.value, 4) + "  kWh  ");
          Serial.print(sampleValues.EndTime_Ms - sampleValues.StartTime_Ms);         
          Serial.print(" ms");
          Serial.println("");
        }

           
}
  

/*
void SERCOM1_Handler()    // Interrupt handler for SERCOM1
{
  Serial2.IrqHandler();
}
*/
