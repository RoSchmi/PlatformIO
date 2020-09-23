#include <Arduino.h>
#include <DateTime.h>
#include <RTC_SAMD51.h>

#ifndef _TIMEFETCHER_H_
#define _TIMEFETCHER_H_

class TimeFetcher
{
public:
    TimeFetcher();
    ~TimeFetcher();
    
    DateTime getTime(); 
};
#endif 