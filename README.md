# PlatformIO
Some Projects made on PlatformIO

### Feather_M0_Reflectance_Sensor_Test
Testing Pololu Reflectance Sensor on Feather M0. Trying to get "On" and "Off" states from the reflectance readings (floating threshold)

### Feather_M0_Debugging_01
Simple Blinky App with Settings for Debugging with Segger J-Link Emulator

### Feather_M0_SDM530MT_Modbus
Reading Voltage from SDM530MT Smartmeter via Modbus - Feather M0 Board - (Debugging with Segger J-Link Emulator)

### Feather_M0_SDM530MT_Modbus_Test
Reading Power and Work from SDM530MT Smartmeter via Modbus - Feather M0 Board - (Debugging with Segger J-Link Emulator)
Using a dedicated Class to read two Input Registers (as needed by the Smartmeter). More easily understandable as the
version above

### Feather_Rfm69_Power_a_Heating_Survey
Reads data (Current, Power, Work) from a Eastron SDM530-MT Smartmeter and the On/Off state of a pump
and transmits these data via Rfm69 radio transmission to an Internet Gateway (GHI FEZ Spider Board)
which stores the data in the Cloud (Azure Storage Tables)

### MoteinoReadAnalogRfm69
App for MoteinoRfm69 board or Arduino pro mini. Reads analog values from three inputs, transforms voltage values to temperature values
and sends values via radio transmission to an Internet Gateway (GHI FEZ Spider Board)

### BrainPad_Blink_Button_Test
Working example to program on PlatformIO for the BrainPad BP2 using SWD-Debugging with Segger J-Link Edu.
Uses the Multicolor LED of the BrainPad and one of the Buttons.
More features of the BrainPad are actually not explored.

### Wio_Terminal_JLink_Debugging
Example shows and explains how I got debugging with JLink on Seeed Wio Terminal working.
There were some hacks needed to get it working

### Wio_Terminal_Seeed_Https_Example
This is the example 'Reading Github Repositoy Stats from Wio Terminal' by Seeed Studio with minor modifications. It shows how to access internet resource via https protocol.

### Wio_Terminal_Https_Get_Request
Shows secure (https) GET request using the arduino-esp32 HTTPClient
(Origninally posted for esp32 devices: https://techtutorialsx.com/2017/11/18/esp32-arduino-https-get-request )

### Wio_Terminal_Azure_DataSender_02

Shows how to transfere sensor data to Azure Storage Tables. App running on the Wio Terminal developed on PlatformIO using Arduino Code, Microsoft Azure C SDK and espressif arduino-esp32 HTTPClient. This is still work in progress, creating tables and inserting Entities is already working.

