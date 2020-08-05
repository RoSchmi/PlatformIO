#pragma once

#ifndef _DATACONTAINERTEMP_H_
#define _DATACONTAINERTEMP_H_
#endif

#include <inttypes.h>

typedef struct
{
    float  AverageCollectorTemp;
    float AverageStorageTemp;
    float AverageWaterTemp;
    //uint32_t ImportWork;    
    uint32_t StartTime_Ms;
    uint32_t EndTime_Ms; 
}
SampleValues;

enum TriggerDeviationDegrees : int16_t
{
    UNITS_01                     = 1,
    UNITS_02                     = 2,
    UNITS_03                     = 3,
    UNITS_04                     = 4,
    UNITS_05                     = 5,
    UNITS_06                     = 6,
    UNITS_07                     = 7,
    UNITS_08                     = 8,
    UNITS_09                     = 9,
    UNITS_10                     = 10,
    UNITS_11                     = 11,
    UNITS_12                     = 12,
    UNITS_13                     = 13,
    UNITS_14                     = 14,
    UNITS_15                     = 15
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

class DataContainerTemp
    { 
    public:
        // constructor
        DataContainerTemp(uint32_t pMaxSendInterval, TriggerDeviation pPercentDeviationPercent, TriggerDeviationDegrees pDeviationDegree);

        void SetNewValues(uint32_t pActSampleTime, float pActCollectorTemp, float pActStorageTemp, float pActWaterTemp);

        bool hasToBeSent();

        SampleValues getSampleValuesAndReset();

       
    private:
      volatile uint32_t ActSampleTime_Ms;
      volatile uint32_t LastSampleTime_Ms;
      volatile uint32_t LastSendTime_Ms;
      uint32_t AveragingTimespan_Ms;
      uint32_t MaxSendInterval_Ms;
      TriggerDeviation PercentDeviationLevel;
      TriggerDeviationDegrees UnitsDeviationLevel;

      float LastAverageCollectorTemp;
      float ActAverageCollectorTemp;
      float ActMeasuredCollectorTemp;
      float SummedCollectorTemp;

      float LastAverageStorageTemp;
      float ActAverageStorageTemp;
      float ActMeasuredStorageTemp;
      float SummedStorageTemp;

      float LastAverageWaterTemp;
      float ActAverageWaterTemp;
      float ActMeasuredWaterTemp;
      float SummedWaterTemp;

      //uint32_t ActImportWorkUint32;

      volatile bool _hasToBeSent;
      bool isFirstTransmission;

      SampleValues sampleValues;

    };