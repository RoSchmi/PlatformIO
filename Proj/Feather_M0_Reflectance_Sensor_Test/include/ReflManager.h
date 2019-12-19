#pragma once

#include <stdint.h>

enum class ReflState : uint8_t {
  On,
  Off,
  Undefined
};

class ReflManager
{
    
    private:
    volatile bool firstReceived = false;
    volatile bool firstSwitchOccurred = false;
    volatile ReflState actState = ReflState::Undefined;
    volatile uint16_t sensorVals[6];
    
    uint16_t highSensorVals[6] {0,0,0,0,0,0};
    
    uint16_t lowSensorVals[6] {0,0,0,0,0,0};
    
    volatile ReflState reflState;

    volatile double maxAverage; 
    volatile double minAverage;
    double averageOf_6_Array(uint16_t pArray[6]);

    uint16_t thresholdPreset = 3;
    uint16_t thresholdDivisor = 3;

    double thresholdDiff = 2;

    public:
    ReflManager(uint16_t pThresholdPreset, uint16_t pThresholdDivisor);
    ReflState setNewReflValue(uint16_t value);
};