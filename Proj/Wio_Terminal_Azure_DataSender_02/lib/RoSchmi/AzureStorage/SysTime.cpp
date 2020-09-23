#include <Arduino.h>
#include <AzureStorage/SysTime.h>

static bool rtcIsStarted = false;

static RTC_SAMD51 rtc;

SysTime::SysTime()
{ 
    rtcIsStarted = true;
    rtc.begin();
}

SysTime::~SysTime(){};

void SysTime::begin(DateTime dateTime)
{   
    if (!rtcIsStarted)
    {
        rtc.begin();
    }
    rtc.adjust(dateTime);   
}
DateTime SysTime::getTime()
{    
    if (!rtcIsStarted)
    {
        rtc.begin();
    }
    return rtc.now();
}
