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
