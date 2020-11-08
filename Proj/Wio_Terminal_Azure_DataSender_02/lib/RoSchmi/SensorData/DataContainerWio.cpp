#include <Arduino.h>
#include "DataContainerWio.h"
#include "math.h"
#include "stdlib.h"
#include "DateTime.h"

DataContainerWio::DataContainerWio(TimeSpan pSendInterval, TimeSpan pInvalidateInterval, float pLowerLimit, float pUpperLimit, float pMagicNumberInvalid)
{
    SendInterval = pSendInterval,
    InvalidateInterval = pInvalidateInterval;
    LowerLimit = pLowerLimit;
    UpperLimit = pUpperLimit;
    MagicNumberInvalid = pMagicNumberInvalid;

    
    /*
    MaxSendInterval_Ms = pMaxSendInterval,

    PercentDeviationLevel = pPercentDeviationPercent,
    UnitsDeviationLevel = pDeviationDegrees,
    ActSampleTime_Ms = 0,
    LastSampleTime_Ms = 0,
    LastSendTime_Ms = 0,
    SampleCounter = 0;  
    AveragingTimespan_Ms = 60000,
    LastAverageCollectorTemp = 0,
    ActAverageCollectorTemp = 0,
    SummedCollectorTemp = 0,
    LastAverageStorageTemp = 0,
    ActAverageStorageTemp = 0,
    SummedStorageTemp = 0,
    LastAverageWaterTemp = 0,
    ActAverageWaterTemp = 0,
    SummedWaterTemp = 0,  
    _hasToBeSent = false,
    isFirstTransmission = true;
    */
}

void DataContainerWio::SetNewValue(uint32_t pIndex, DateTime pActDateTime, float pSampleValue)
{
    SampleValues[pIndex].Value = pSampleValue;
    SampleValues[pIndex].LastSendTime = pActDateTime;

    if (isFirstTransmission)
    {
        //isFirstTransmission = false;
        _hasToBeSent = true;
        _lastSentTime = pActDateTime;         
    }
    else
    {
        if (_lastSentTime.operator<=(pActDateTime.operator-(SendInterval)))
        {
            _hasToBeSent = true;
            

        }
        /*
        else
        {
            _hasToBeSent = false;
        }
        */
    }
    
}
/*
void DataContainerTemp::SetNewValues(uint32_t pActSampleTime, float pActCollectorTemp, float pActStorageTemp, float pActWaterTemp)
{
    if (isFirstTransmission)
    {
        SampleCounter++;

        LastSampleTime_Ms = pActSampleTime - 1;   
        LastSendTime_Ms = pActSampleTime - 1;

        ActAverageCollectorTemp = pActCollectorTemp;       
        LastAverageCollectorTemp = pActCollectorTemp;       
        SummedCollectorTemp = pActCollectorTemp;

        ActAverageStorageTemp = pActStorageTemp;        
        LastAverageStorageTemp = pActStorageTemp;              
        SummedStorageTemp = pActStorageTemp;

        ActAverageWaterTemp = pActWaterTemp;        
        LastAverageWaterTemp = pActWaterTemp;             
        SummedWaterTemp = pActWaterTemp;

        isFirstTransmission = false;
        _hasToBeSent = true;
    }
    else
    {
        SampleCounter++;
        
        SummedCollectorTemp += pActCollectorTemp;     
        ActAverageCollectorTemp = SummedCollectorTemp / (float)SampleCounter;
        
        SummedStorageTemp += pActStorageTemp;       
        ActAverageStorageTemp = SummedStorageTemp / (float)SampleCounter;
        
        SummedWaterTemp += pActWaterTemp;      
        ActAverageWaterTemp = SummedWaterTemp / (float)SampleCounter;

        LastSampleTime_Ms = ActSampleTime_Ms;  
    }   
    ActSampleTime_Ms = pActSampleTime;
    //RoSchmi, for debugging
        //
        //Serial.print("Calculation: ");
        //Serial.print(SummedCollectorTemp);
        //Serial.print(" ");
        //Serial.print(SampleCounter);
        //Serial.print(" ");
        //Serial.println(ActAverageCollectorTemp);
        //
    ActMeasuredCollectorTemp = pActCollectorTemp;   
    ActMeasuredStorageTemp = pActStorageTemp;   
    ActMeasuredWaterTemp = pActWaterTemp;
  
    if((ActSampleTime_Ms - LastSendTime_Ms) > MaxSendInterval_Ms)
    {
        // RoSchmi, for debugging
            //
            //Serial.print("Timecalculation: ");
            //Serial.print(ActSampleTime_Ms - LastSendTime_Ms);
            //Serial.print(" ");
            //Serial.println(MaxSendInterval_Ms);
            //
        LastAverageCollectorTemp = ActAverageCollectorTemp;      
        LastAverageStorageTemp = ActAverageStorageTemp;
        LastAverageWaterTemp = ActAverageWaterTemp;
        _hasToBeSent = true;
    }

    float averageCollectorTempDiff = (ActAverageCollectorTemp - LastAverageCollectorTemp);
    
    // Outcommented variables used for debugging
    // volatile int16_t thePercentLevel = (int16_t)PercentDeviationLevel;
    // volatile float theLevel = LastAverageCurrent / 100 * (int16_t)PercentDeviationLevel;
    // volatile float theValue = abs(averageCurrentDiff);
    // volatile float theAmpsDeviation = ((int16_t)AmpereDeviationLevel) * 0.1;

    // volatile bool Condition_Time = ((ActSampleTime_Ms - LastSendTime_Ms) > AveragingTimespan_Ms);
    // volatile bool ConditionPercent = (abs(averageCurrentDiff)  > (LastAverageCurrent / 100 * (int16_t)PercentDeviationLevel));
    // volatile bool ConditionAmpere = (abs(averageCurrentDiff) > (((int16_t)AmpereDeviationLevel) * 0.1));

    // --> send if AveragingTimespan is exceeded and a deviation to the value before is of a selected percent or an absolute Ampere value
    //if (((int32_t)(ActSampleTime_Ms - LastSendTime_Ms) > (int32_t)AveragingTimespan_Ms) && 
    //   (abs(averageCurrentDiff)  > (LastAverageCurrent / 100 * (int16_t)PercentDeviationLevel)) || 
    //   (abs(averageCurrentDiff) > (((int16_t)AmpereDeviationLevel) * 0.1)))


    if (((ActSampleTime_Ms - LastSendTime_Ms) > AveragingTimespan_Ms) && 
       ((abs(averageCollectorTempDiff)  > (LastAverageCollectorTemp / 100 * (int16_t)PercentDeviationLevel)) || 
       (abs(averageCollectorTempDiff) > (((int16_t)UnitsDeviationLevel) * 1.0))))

    {
        LastAverageCollectorTemp = ActAverageCollectorTemp;
        LastAverageStorageTemp = ActAverageStorageTemp;
        _hasToBeSent = true;
    }
}
*/

void DataContainerWio::setUpperLimit(float pUpperLimit)
{
    UpperLimit = pUpperLimit;
}

void DataContainerWio::setLowerLimit(float pLowerLimit)
{
    LowerLimit = pLowerLimit;
}

void DataContainerWio::setMagigNumberInvalid(float pMagicNumberInvalid)
{
   MagicNumberInvalid = pMagicNumberInvalid;
}

bool DataContainerWio::hasToBeSent()
{
    return _hasToBeSent;
}

SampleValueSet DataContainerWio::getSampleValuesAndReset(DateTime pActDateTime)
{
    _hasToBeSent = false;
    isFirstTransmission = false;
    _SampleValuesSet.LastSendTime = _lastSentTime;
    _lastSentTime = pActDateTime;
    _SampleValuesSet.SampleValues[0] = SampleValues[0];
    _SampleValuesSet.SampleValues[1] = SampleValues[1];
    _SampleValuesSet.SampleValues[2] = SampleValues[2];
    _SampleValuesSet.SampleValues[3] = SampleValues[3];
    return _SampleValuesSet;
}


/*
SampleValues DataContainerTemp::getSampleValuesAndReset()
{
    sampleValues.AverageCollectorTemp = ActAverageCollectorTemp;
    sampleValues.AverageStorageTemp = ActAverageStorageTemp;
    sampleValues.AverageWaterTemp = ActAverageWaterTemp;  
    sampleValues.StartTime_Ms = LastSendTime_Ms;
    sampleValues.EndTime_Ms = ActSampleTime_Ms;
    SampleCounter = 0;  
    LastSendTime_Ms = ActSampleTime_Ms;
    SummedCollectorTemp = 0;
    SummedStorageTemp = 0;
    SummedWaterTemp = 0;
    _hasToBeSent = false;
    return sampleValues;
}
*/

