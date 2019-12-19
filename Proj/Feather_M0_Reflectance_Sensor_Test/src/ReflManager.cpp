#include <Arduino.h>
#include <ReflManager.h>
#include <stdint.h>

using namespace std;

// Constructor
ReflManager::ReflManager(uint16_t pThresholdPreset, uint16_t pThresholdDivisor)
{
   thresholdPreset = pThresholdPreset;
   thresholdDivisor = pThresholdDivisor;
}

ReflState ReflManager::setNewReflValue(uint16_t value)
{       
    if (!firstReceived)   // At start fill array with first value
    {  
        Serial.print("Initialize Array");
        firstReceived = true;       
        for (int i = 0; i < 6; i++)
        {
           sensorVals[i] = value;
        }       
    }
    
    for (int i = 5; i > 0; i--)       // Shift values in array and store value in position [0]
    {
        sensorVals[i] = sensorVals[i -1];
    }
    sensorVals[0] = value;

    // calculate average of the 3 oldest values in arry and of 3 youngest values in array
    double newAverage = (sensorVals[0] + sensorVals[1] + sensorVals[2]) / 3;
    double oldAverage = (sensorVals[3] + sensorVals[4] + sensorVals[5]) / 3;
    
    // find out which of both is higher and store to corresponding variable 
    double highAverage =  oldAverage > newAverage ? oldAverage : newAverage;
    double lowAverage = oldAverage > newAverage ? newAverage : oldAverage;
       
    if (actState == ReflState::On)    // actualize corresponding array for state = "On"
    {      
          maxAverage = highAverage > maxAverage ? highAverage : maxAverage;
          for (int i = 5; i > 0 ; i--)
          {
              highSensorVals[i] = highSensorVals[i - 1];
          }
          highSensorVals[0] = highAverage;
          maxAverage--;
          minAverage = highAverage -1;
    }
    if (actState == ReflState::Off)     // actualize corresponding array for state = "Off"
    {  
        minAverage = lowAverage < minAverage ? lowAverage : minAverage;
        for (int i = 5; i > 0 ; i--)
          {
              lowSensorVals[i] = lowSensorVals[i - 1];
          }
        lowSensorVals[0] = lowAverage; 
        minAverage++;
        maxAverage = lowAverage +1;
    }
    
    if (!firstSwitchOccurred)   // used only for the first switch
    {
        if ((abs(oldAverage - newAverage)) > thresholdPreset)
        {              
            if ((oldAverage - newAverage) > thresholdPreset)
            {          
                minAverage = newAverage;
                for (int i = 0; i < 6; i++)
                {
                    lowSensorVals[i] = newAverage;
                    highSensorVals[i] = oldAverage;
                }
                actState = ReflState::Off;           
            }
            else
            {               
                maxAverage = newAverage;
                for (int i = 0; i < 6; i++)
                {
                    highSensorVals[i] = newAverage;
                    lowSensorVals[i] = oldAverage;
                }      
                actState = ReflState::On;           
            }
            firstSwitchOccurred = true;          
            return actState;    
        }
        else
        {                            
            return actState;
        }
       
    }
    else             // used for following switches
    {  
        thresholdDiff = (averageOf_6_Array(highSensorVals) - averageOf_6_Array(lowSensorVals)) / thresholdDivisor;
       
        if ((actState == ReflState::On) && (newAverage < (maxAverage - (thresholdDiff < 2 ? 2 : thresholdDiff))))
        {
            minAverage = newAverage; 
            actState = ReflState::Off;
        }
       
        if ((actState == ReflState::Off) && (newAverage > (minAverage + (thresholdDiff < 2 ? 2 : thresholdDiff))))
        {
            maxAverage = newAverage;
            actState = ReflState::On;
        } 
        Serial.print("MaxAverage: ");
        Serial.print(int(maxAverage));       
        Serial.print("  ");
        Serial.print("MinAverage: ");
        Serial.print(int(minAverage));
        Serial.print("  ");
        Serial.print("NewAverage: ");
        Serial.print((int)newAverage);
        Serial.print("  ");
        Serial.print("  ");
        Serial.print("Diff: ");
        Serial.print((int)thresholdDiff);
        Serial.print("  ");
        if (actState == ReflState::On)
        {
            Serial.println("On  ================================");
        }
        else
        {
            Serial.println("Off  ==");
        }        
        return actState;
    }       
}
double ReflManager::averageOf_6_Array(uint16_t pArray[6])
{
    double retValue = 0;
    for (int i = 0; i < 6; i++)
    {
        retValue += pArray[i];
    }
    return retValue / 6;
}