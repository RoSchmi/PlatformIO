#include "DataContainer.h"
#include <math.h>

DataContainer::DataContainer(uint32_t pMaxSendInterval, TriggerDeviation pPercentDeviationPercent, TriggerDeviationAmps pDeviationAmpere)
{
    MaxSendInterval_Ms = pMaxSendInterval,
    PercentDeviationLevel = pPercentDeviationPercent,
    AmpereDeviationLevel = pDeviationAmpere,
    ActSampleTime_Ms = 0,
    LastSampleTime_Ms = 0,
    LastSendTime_Ms = 0,
    SampleCounter = 0,
    AveragingTimespan_Ms = 60000,
    LastAverageCurrent = 0,
    ActAverageCurrent = 0,
    SummedCurrents = 0,
    LastAveragePower = 0,
    ActAveragePower = 0,
    SummedPower = 0,
    ActImportWorkUint32 = 0,
    _hasToBeSent = false,
    isFirstTransmission = true;
}

void DataContainer::SetNewValues(uint32_t pActSampleTime, float pActCurrent, float pActPower, uint32_t pActImportWorkUint32, bool forceSend)
{
    if (isFirstTransmission)
    {
        SampleCounter++;
        LastSampleTime_Ms = pActSampleTime - 1;       
        LastSendTime_Ms = pActSampleTime - 1;

        ActAverageCurrent = pActCurrent;
        LastAverageCurrent = pActCurrent;
        //SummedCurrents = pActCurrent * (pActSampleTime - LastSampleTime_Ms);
        SummedCurrents = pActCurrent;

        ActAveragePower = pActPower;
        LastAveragePower = pActPower;
        //SummedPower = pActPower  * (pActSampleTime - LastSampleTime_Ms);
        SummedPower = pActPower;
        isFirstTransmission = false;
        _hasToBeSent = true;
    }
    else
    {
        SampleCounter++;
        //SummedCurrents += pActCurrent * (pActSampleTime - LastSampleTime_Ms);
        SummedCurrents += pActCurrent;     
        //ActAverageCurrent = SummedCurrents / (pActSampleTime - LastSendTime_Ms);
        ActAverageCurrent = SummedCurrents / (float)SampleCounter;

        //SummedPower += pActPower * (pActSampleTime - LastSampleTime_Ms);
        SummedPower += pActPower;
        //ActAveragePower = SummedPower / (pActSampleTime - LastSendTime_Ms);
        ActAveragePower = SummedPower / (float)SampleCounter;
        
        LastSampleTime_Ms = ActSampleTime_Ms;
        
    }   
    ActSampleTime_Ms = pActSampleTime;

    ActMeasuredCurrent = pActCurrent;
    ActMeasuredPower = pActPower;
    ActImportWorkUint32 = pActImportWorkUint32;
  
    if((ActSampleTime_Ms - LastSendTime_Ms) > MaxSendInterval_Ms)
    {
        LastAverageCurrent = ActAverageCurrent;
       
        LastAveragePower = ActAveragePower;
        _hasToBeSent = true;
    }

    float averageCurrentDiff = (ActAverageCurrent - LastAverageCurrent);

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

    if (forceSend)
    {
        LastAverageCurrent = ActAverageCurrent;
        LastAveragePower = ActAveragePower;

        ActAverageCurrent = ActMeasuredCurrent;    // set ActAverage to actual values if 'forced'
        ActAveragePower = ActMeasuredPower;
        _hasToBeSent = true;
    }
    else
    {

        if (((ActSampleTime_Ms - LastSendTime_Ms) > AveragingTimespan_Ms) && 
            ((abs(averageCurrentDiff)  > (LastAverageCurrent / 100 * (int16_t)PercentDeviationLevel)) || 
            (abs(averageCurrentDiff) > (((int16_t)AmpereDeviationLevel) * 0.1))))

        {
            LastAverageCurrent = ActAverageCurrent;
            LastAveragePower = ActAveragePower;
            _hasToBeSent = true;
        }
    }
}

bool DataContainer::hasToBeSent()
{
    return _hasToBeSent;
}

SampleValues DataContainer::getSampleValuesAndReset()
{
    sampleValues.AverageCurrent = ActAverageCurrent;
    sampleValues.AveragePower = ActAveragePower;
    sampleValues.ImportWork = ActImportWorkUint32;
    sampleValues.StartTime_Ms = LastSendTime_Ms;
    sampleValues.EndTime_Ms = ActSampleTime_Ms;
    SampleCounter = 0; 
    LastSendTime_Ms = ActSampleTime_Ms;
    SummedCurrents = 0;
    SummedPower = 0;
    _hasToBeSent = false;
    return sampleValues;
}

