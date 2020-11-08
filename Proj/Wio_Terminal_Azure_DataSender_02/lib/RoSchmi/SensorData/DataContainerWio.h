#include <Arduino.h>
#include <Datetime.h>


#ifndef _DATACONTAINERWIO_H_
#define _DATACONTAINERWIO_H_

#define PROPERTY_COUNT 4
//#define MAX_VALUE 140
//#define MIN_VALUE -40

typedef struct
{
    float  Value = 999.9;
    DateTime LastSendTime;
}
SampleValue; 
    
typedef struct
{  
    DateTime LastSendTime;
    SampleValue SampleValues[PROPERTY_COUNT];
}
SampleValueSet; 

class DataContainerWio
{

public:
    
    DataContainerWio(TimeSpan pSendInterval, TimeSpan pInvalidateInterval, float pLowerLimit, float pUpperLimit, float pMagicNumberInvalid); 
    //~DataContainerWio();

    void SetNewValue(uint32_t pIndex, DateTime pActDateTime, float pSampleValue);
    SampleValueSet getSampleValuesAndReset(DateTime pActDateTime);
    bool hasToBeSent();
    void setLowerLimit(float pLowerLimit);
    void setUpperLimit(float pUpperLimit);
    void setMagigNumberInvalid(float pMagicNumberInvalid);

    bool isFirstTransmission = true;

    TimeSpan SendInterval;
    TimeSpan InvalidateInterval;
    float LowerLimit = -40.0;
    float UpperLimit = 140.0;
    float MagicNumberInvalid = 999.9;

    bool _hasToBeSent = false;

    DateTime _lastSentTime;
    SampleValue SampleValues[PROPERTY_COUNT];
    SampleValueSet _SampleValuesSet;
};


#endif  // _DATACONTAINERWIO_H_