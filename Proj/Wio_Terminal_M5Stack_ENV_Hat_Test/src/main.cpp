// Application: Wio_Terminal_M5Stack_ENV_Hat_Test
// Shows how to read all sensores of M5Stack ENV II Hat (temperature, humidity, atmospheric pressure and magnetic field)
// on Wio Terminal

// Note:
// In file bmm150_defs.h change I2C Address to 0x10
//
//  #define BMM150_I2C_Address  0x13    // Original value
//  #define BMM150_I2C_Address  0x10    // New value

#include <Arduino.h>
#include "SHT31.h"
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>
#include "bmm150.h"
#include "bmm150_defs.h"

SHT31 sht31 = SHT31();

BMM150 bmm = BMM150();
bmm150_mag_data value_offset;

Adafruit_BMP280 bme;

#define M_PI 3.142
void calibrate(uint32_t timeout)
{
  int16_t value_x_min = 0;
  int16_t value_x_max = 0;
  int16_t value_y_min = 0;
  int16_t value_y_max = 0;
  int16_t value_z_min = 0;
  int16_t value_z_max = 0;
  uint32_t timeStart = 0;

  bmm.read_mag_data();  
  value_x_min = bmm.raw_mag_data.raw_datax;
  value_x_max = bmm.raw_mag_data.raw_datax;
  value_y_min = bmm.raw_mag_data.raw_datay;
  value_y_max = bmm.raw_mag_data.raw_datay;
  value_z_min = bmm.raw_mag_data.raw_dataz;
  value_z_max = bmm.raw_mag_data.raw_dataz;
  delay(100);

  timeStart = millis();
  
  while((millis() - timeStart) < timeout)
  {
    bmm.read_mag_data();
    
    // Update x-Axis max/min value 
    if(value_x_min > bmm.raw_mag_data.raw_datax)
    {
      value_x_min = bmm.raw_mag_data.raw_datax;
       Serial.print("Update value_x_min: ");
       Serial.println(value_x_min);

    } 
    else if(value_x_max < bmm.raw_mag_data.raw_datax)
    {
      value_x_max = bmm.raw_mag_data.raw_datax;
       Serial.print("update value_x_max: ");
       Serial.println(value_x_max);
    }

    // Update y-Axis max/min value 
    if(value_y_min > bmm.raw_mag_data.raw_datay)
    {
      value_y_min = bmm.raw_mag_data.raw_datay;
       Serial.print("Update value_y_min: ");
       Serial.println(value_y_min);

    } 
    else if(value_y_max < bmm.raw_mag_data.raw_datay)
    {
      value_y_max = bmm.raw_mag_data.raw_datay;
       Serial.print("update value_y_max: ");
       Serial.println(value_y_max);
    }

    // Update z-Axis max/min value 
    if(value_z_min > bmm.raw_mag_data.raw_dataz)
    {
      value_z_min = bmm.raw_mag_data.raw_dataz;
       Serial.print("Update value_z_min: ");
       Serial.println(value_z_min);

    } 
    else if(value_z_max < bmm.raw_mag_data.raw_dataz)
    {
      value_z_max = bmm.raw_mag_data.raw_dataz;
       Serial.print("update value_z_max: ");
       Serial.println(value_z_max);
    }
    
    Serial.print(".");
    delay(1);

  }

  value_offset.x = value_x_min + (value_x_max - value_x_min)/2;
  value_offset.y = value_y_min + (value_y_max - value_y_min)/2;
  value_offset.z = value_z_min + (value_z_max - value_z_min)/2;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  
  Serial.println("Starting");
  
  if (!bme.begin(0x76)){
      Serial.println("Could not find a valid BMP280 sensor, check wiring!");
      while (1);
  }
  else
  {
    Serial.println("Found BMP280 sensor");
  }

  volatile float t = bme.readTemperature();

if(bmm.initialize() == BMM150_E_ID_NOT_CONFORM) {
    Serial.println("Chip ID can not read!");
    while(1);
  } else {
    Serial.println("Initialize done!");
  }

  calibrate(10);
  Serial.print("\n\rCalibrate done..");

  sht31.begin(0x44);

  uint8_t setup_flag = 1;
}

void loop() {

  bool result = sht31.read();

  volatile float tmp = sht31.getTemperature();
  volatile float hum = sht31.getHumidity();

  String tmpString = String(tmp, 1);
  String humString = String(hum, 1);

  Serial.printf("\r\nTemp: %s Humi: %s \r\n", tmpString.c_str(), humString.c_str());

  bmm150_mag_data value;
  bmm.read_mag_data();

  value.x = bmm.raw_mag_data.raw_datax - value_offset.x;
  value.y = bmm.raw_mag_data.raw_datay - value_offset.y;
  value.z = bmm.raw_mag_data.raw_dataz - value_offset.z;

  float xyHeading = atan2(value.x, value.y);
  float zxHeading = atan2(value.z, value.x);
  float heading = xyHeading;

  if(heading < 0)
    heading += 2*PI;
  if(heading > 2*PI)
    heading -= 2*PI;
  float headingDegrees = heading * 180/M_PI; 
  float xyHeadingDegrees = xyHeading * 180 / M_PI;
  float zxHeadingDegrees = zxHeading * 180 / M_PI;

  Serial.print("Heading: ");
  Serial.println(headingDegrees);
  Serial.print("xyHeadingDegrees: ");
  Serial.println(xyHeadingDegrees);
  Serial.print("zxHeadingDegrees: ");
  Serial.println(zxHeadingDegrees);

  

  float pressure = bme.readPressure();

  Serial.printf("\r\nPressure: %d ", (int)pressure);
  
  delay(1000);
}