# Feather_Rfm69_Power_a_Heating_Survey

Reads data (Current, Power, Work) from a Eastron SDM530-MT Smartmeter and the On/Off state of a pump
and transmits these data via Rfm69 radio transmission to an Internet Gateway (GHI FEZ Spider Board)
which stores the data in the Cloud (Azure Storage Tables)

There were difficulties to debug with JLink, this link shows how it can be done.

https://community.platformio.org/t/problems-starting-debug-session-with-jlink-on-feather-m0/12291

To work without bootloader as suggested in this link, the BooProt Fuse has to be set to 0 bytes
with Atmel Studio. The special .json file has to be in the folder 'boards'
