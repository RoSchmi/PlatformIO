#pragma once

#ifndef _DATACONTAINER_H_
#define _DATACONTAINER_H_
#endif

#include <inttypes.h>

typedef struct
{
    float  AverageCurrent;
    float AveragePower;
    uint32_t ImportWork;    
    uint32_t StartTime_Ms;
    uint32_t EndTime_Ms; 
}
SampleValues;

enum TriggerDeviationAmps : int16_t
{
    AMPERE_01                     = 1,
    AMPERE_02                     = 2,
    AMPERE_03                     = 3,
    AMPERE_04                     = 4,
    AMPERE_05                     = 5,
    AMPERE_06                     = 6,
    AMPERE_07                     = 7,
    AMPERE_08                     = 8,
    AMPERE_09                     = 9,
    AMPERE_10                     = 10  
};


enum TriggerDeviation : int16_t
{
    PERCENT_01                     = 1,
    PERCENT_02                     = 2,
    PERCENT_05                     = 5,
    PERCENT_07                     = 7,
    PERCENT_10                     = 10,
    PERCENT_20                     = 20,
    PERCENT_30                     = 30,
    PERCENT_40                     = 40,
    PERCENT_50                     = 50,
    PERCENT_60                     = 60,
    PERCENT_70                     = 70,
    PERCENT_80                     = 80,
    PERCENT_90                     = 90,
    PERCENT_100                    = 100,
    PERCENT_150                    = 150,
    PERCENT_200                    = 200  
};

class DataContainer
    { 
    public:
        // constructor
        DataContainer(uint32_t pMaxSendInterval, TriggerDeviation pPercentDeviationPercent, TriggerDeviationAmps pDeviationAmpere);

        void SetNewValues(uint32_t pActSampleTime, float pActCurrent, float pActPower, uint32_t pActImportWorkUint32, bool forceSend = false);

        bool hasToBeSent();

        SampleValues getSampleValuesAndReset();

       
    private:
      volatile uint32_t ActSampleTime_Ms;
      volatile uint32_t LastSampleTime_Ms;
      volatile uint32_t LastSendTime_Ms;
      uint32_t AveragingTimespan_Ms;
      uint32_t MaxSendInterval_Ms;

      uint32_t SampleCounter; 

      TriggerDeviation PercentDeviationLevel;
      TriggerDeviationAmps AmpereDeviationLevel;

      float LastAverageCurrent;
      float ActAverageCurrent;
      float ActMeasuredCurrent;
      float SummedCurrents;

      float LastAveragePower;
      float ActAveragePower;
      float ActMeasuredPower;
      float SummedPower;

      uint32_t ActImportWorkUint32;

      volatile bool _hasToBeSent;
      bool isFirstTransmission;

      SampleValues sampleValues;

    };