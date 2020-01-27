// Copyright: RoSchmi 2020 License: GNU Lesser General Public License v2.1
// This is a simple Modubus transmission example
// The code runs on a Feather M0 board
// The program reads Input Registers (Voltage) from a Eastron SDM530-MT Smartmeter
// The Modbus libraries are from
// https://github.com/mcci-catena/Modbus-for-Arduino
//


#include <Arduino.h>
#include <stdint.h>
#include "wiring_private.h" // pinPeripheral() function
#include "variant.h"
#include <ModbusRtu.h>
#include "Reform_uint16_2_float32.h"

// data array for modbus network sharing
uint16_t au16data[16];
u_int8_t u8state;
uint8_t u8addr;
uint8_t u8lastaddr;

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

unsigned long u32wait;

// Serial2 pin and pad definitions (in Arduino files Variant.h & Variant.cpp)
#define PIN_SERIAL2_RX       (34ul)               // unsigned long (32 bit )Pin description number for PIO_SERCOM on D12 - PA19
#define PIN_SERIAL2_TX       (36ul)               // unsigned long (32 bit )Pin description number for PIO_SERCOM on D10 - PA18
#define PAD_SERIAL2_TX       (UART_TX_PAD_2)      // SERCOM pad 2
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_3)    // SERCOM pad 3

// Instantiate the Serial2 class
Uart Serial2(&sercom1, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX);

/**
 *  Modbus object declaration
 *  u8id : node id = 0 for host, = 1..247 for device
 *  u8serno : serial port (use 0 for Serial)
 *  u8txenpin : 0 for RS-232 and USB-FTDI 
 *               or any pin number > 1 for RS-485
 */
Modbus host(0, A4); // this is host and RS-232 or USB-FTDI

// ModbusSerial<decltype(Serial1)> mySerial(&Serial1);
 ModbusSerial<decltype(Serial2)> mySerial(&Serial2);

void setup() {
  // put your setup code here, to run once:
   powerOn();
  host.begin(&mySerial, 2400, SERIAL_8N1); // baud-rate at 2400
  host.setTimeOut( 1000 ); // if there is no answer in 1000 ms, roll over

  host.setTxEnableDelay(100);

  u32wait = millis() + 1000;
  u8state = 0; 
  u8addr = 1;
  u8lastaddr = 1;
  while (!Serial); // wait until serial console is open, remove if not tethered to computer
  Serial.begin(115200);
  Serial.println("Hallo"); 
}

void loop() 
{
  // put your main code here, to run repeatedly:
      switch( u8state ) 
      {
        case 0:
        if (long(millis() - u32wait) > 0) 
        {
          u8state++; // wait state
        }
        break;
        case 1: 
        datagram.u8id = u8addr; // device address 
        datagram.u8fct = 4; // function code (this one is Input registers read)     
        datagram.u16RegAdd = 0x0000; // start address in device        
        datagram.u16CoilsNo = 2; // number of elements (coils or registers) to read
        datagram.au16reg = au16data; // pointer to a memory array in the Arduino
        host.setLastError(ERR_SUCCESS);
        host.query( datagram ); // send query (only once)
        u8state++;        
        break;
        case 2:
        // This delay is needed, otherwise often corrupted transmission
        delay(10); 
        host.poll(); // check incoming messages
        if (host.getState() == COM_IDLE) 
        {
          u8state = 0;
          ERR_LIST lastError = host.getLastError();
          if (host.getLastError() != ERR_SUCCESS) 
          {            
	          Serial.print(": Error ");
	          Serial.print(int(lastError));           
            u32wait = millis() + 100;
          } 
          else 
          {
            Serial.print(millis());
            Serial.print(": addr=");
            Serial.print(u8addr, 16);
            Serial.print(": Registers: ");
            for (int i=0; i < 4; ++i)
            {
              Serial.print(" ");
              Serial.print(au16data[i], 16);                             
            }
            // Calculate value as float and print out
            int16_2_float_function_result resultStruct = reform_uint16_2_float32(au16data[0], au16data[1]);
            if (resultStruct.validity == true)
            {
              Serial.print("  " + String(resultStruct.value, 2) + "  Volts");             
            }
            else
            {
              Serial.print("  Not valid"); 
            }  

            // Blink a LED 
            digitalWrite(PIN_LED_13, HIGH); // turn the LED on by making the voltage HIGH           
            delay(100);              // wait for a time
            digitalWrite(PIN_LED_13, LOW); // turn the LED off by making the voltage LOW           
                                      
            u32wait = millis() + 1000;     // Read again after this time        
          }
          Serial.println("");
        }       
        break;     
      }      
}
  

void SERCOM1_Handler()    // Interrupt handler for SERCOM1
{
  Serial2.IrqHandler();
}
