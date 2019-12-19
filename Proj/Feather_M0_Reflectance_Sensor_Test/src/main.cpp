#include <Arduino.h>

#include <QTRSensors.h>
#include <ReflManager.h>

// This example reads the Reflectance values (between 0 and 1000) through
// the Pololu library QTRSensor
// Then the sensorValue is passed to the ReflManager library
// If no change of the reflectance has occurred "ReflState::Undefined" is returned
// After the first time a certain thershold is exceeded, "ReflState::On" or "ReflState::Off"
// are returned, switching the corresponding LED "On" or "Off"


// Explantions from library to read Pololu Refelctance Sensor
//
// This example is designed for use with six analog QTR sensors. These
// reflectance sensors should be connected to analog pins A0 to A5. The
// sensors' emitter control pin (CTRL or LEDON) can optionally be connected to
// digital pin 2, or you can leave it disconnected and remove the call to
// setEmitterPin().
//
// The main loop of the example reads the raw sensor values (uncalibrated). You
// can test this by taping a piece of 3/4" black electrical tape to a piece of
// white paper and sliding the sensor across it. It prints the sensor values to
// the serial monitor as numbers from 0 (maximum reflectance) to 1023 (minimum
// reflectance).


// Defines the initial threshold for the first switch
// If sensor values difference is more then 5 it is assumed that the state has changed
const uint16_t ThresholdPreset = 5;

// Defines the threshold of following state switches
// The span/difference) between average "On" values and average "Off" values is divided
// by this ThresholdDivisor. If new values deviate by more than this threshold
// from the average values before, a change of the state is assumed 
// (2 = half of the span, 3 = third of the span, 4 = 25% of the span)
const uint16_t ThresholdDivisor = 3;

QTRSensors qtr;
ReflManager reflManager(ThresholdPreset, ThresholdDivisor);

const uint8_t SensorCount = 1;
uint16_t sensorValues[SensorCount];

void setup()
{
  // configure the sensors
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A0, A1, A2, A3, A4, A5}, SensorCount);

  qtr.setEmitterPin(12);

  pinMode(13,OUTPUT);

  //Serial.begin(9600);
  Serial.begin(115200);
 
}


void loop()
{
  // read raw sensor values
  qtr.read(sensorValues);

  // print the sensor values as numbers from 0 to 1023, where 0 means maximum
  // reflectance and 1023 means minimum reflectance
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    
    //volatile bool onOffState = reflManager.setNewReflValue(sensorValues[i]);
    volatile ReflState onOffState = reflManager.setNewReflValue(sensorValues[i]);
    
    if (onOffState == ReflState::On)   
    {
        //Serial.print("On  ");
        digitalWrite(13, HIGH); 
    }
    else
    {
      if (onOffState == ReflState::Off)
      {
        digitalWrite(13, LOW);
        //Serial.print("Off  ");
      }
      else
      {
        digitalWrite(13, LOW);
        Serial.println("Undefinded  ");
      }    
    }
    
    //Serial.print(sensorValues[i]);
    //Serial.print('\t');
  }
  Serial.println();
  delay(100);
}





