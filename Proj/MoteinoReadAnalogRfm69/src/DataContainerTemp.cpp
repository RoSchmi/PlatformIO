#include "DataContainerTemp.h"
#include "math.h"
#include "stdlib.h"

DataContainerTemp::DataContainerTemp(uint32_t pMaxSendInterval, TriggerDeviation pPercentDeviationPercent, TriggerDeviationDegrees pDeviationDegrees)
{
    MaxSendInterval_Ms = pMaxSendInterval,
    PercentDeviationLevel = pPercentDeviationPercent,
    UnitsDeviationLevel = pDeviationDegrees,
    ActSampleTime_Ms = 0,
    LastSampleTime_Ms = 0,
    LastSendTime_Ms = 0,
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


    //ActImportWorkUint32 = 0,
    _hasToBeSent = false,
    isFirstTransmission = true;
}

//void DataContainerTemp::SetNewValues(uint32_t pActSampleTime, float pActCurrent, float pActStorageTemp, uint32_t pActImportWorkUint32)
void DataContainerTemp::SetNewValues(uint32_t pActSampleTime, float pActCollectorTemp, float pActStorageTemp, float pActWaterTemp)
{
    if (isFirstTransmission)
    {
        LastSampleTime_Ms = pActSampleTime - 1;   
        LastSendTime_Ms = pActSampleTime - 1;

        ActAverageCollectorTemp = pActCollectorTemp;       
        LastAverageCollectorTemp = pActCollectorTemp;        
        SummedCollectorTemp = pActCollectorTemp * (pActSampleTime - LastSampleTime_Ms);
        
        ActAverageStorageTemp = pActStorageTemp;        
        LastAverageStorageTemp = pActStorageTemp;      
        SummedStorageTemp = pActStorageTemp  * (pActSampleTime - LastSampleTime_Ms);

        ActAverageWaterTemp = pActWaterTemp;        
        LastAverageWaterTemp = pActWaterTemp;      
        SummedWaterTemp = pActWaterTemp  * (pActSampleTime - LastSampleTime_Ms);

        isFirstTransmission = false;
        _hasToBeSent = true;
    }
    else
    {
       
        SummedCollectorTemp += pActCollectorTemp * (pActSampleTime - LastSampleTime_Ms);             
        ActAverageCollectorTemp = SummedCollectorTemp / (pActSampleTime - LastSendTime_Ms);

        SummedStorageTemp += pActStorageTemp * (pActSampleTime - LastSampleTime_Ms);                
        ActAverageStorageTemp = SummedStorageTemp / (pActSampleTime - LastSendTime_Ms);

        SummedWaterTemp += pActWaterTemp * (pActSampleTime - LastSampleTime_Ms);                
        ActAverageWaterTemp = SummedWaterTemp / (pActSampleTime - LastSendTime_Ms);

        LastSampleTime_Ms = ActSampleTime_Ms;  
    }   
    ActSampleTime_Ms = pActSampleTime;
    //ActMeasuredCurrent = pActCollectorTemp,
    ActMeasuredCollectorTemp = pActCollectorTemp;
    //ActMeasuredPower = pActStorageTemp;
    ActMeasuredStorageTemp = pActStorageTemp;

    //ActImportWorkUint32 = pActImportWorkUint32;
    ActMeasuredWaterTemp = pActWaterTemp;

    //if((int32_t)(ActSampleTime_Ms - LastSendTime_Ms) > (int32_t)MaxSendInterval_Ms)
    if((ActSampleTime_Ms - LastSendTime_Ms) > MaxSendInterval_Ms)
    {
        
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

bool DataContainerTemp::hasToBeSent()
{
    return _hasToBeSent;
}

SampleValues DataContainerTemp::getSampleValuesAndReset()
{
    sampleValues.AverageCollectorTemp = ActAverageCollectorTemp;
    sampleValues.AverageStorageTemp = ActAverageStorageTemp;
    sampleValues.AverageWaterTemp = ActAverageWaterTemp;
    //sampleValues.ImportWork = ActImportWorkUint32;
    sampleValues.StartTime_Ms = LastSendTime_Ms;
    sampleValues.EndTime_Ms = ActSampleTime_Ms;  
    LastSendTime_Ms = ActSampleTime_Ms;
    SummedCollectorTemp = 0;
    SummedStorageTemp = 0;
    SummedWaterTemp = 0;
    _hasToBeSent = false;
    return sampleValues;
}

