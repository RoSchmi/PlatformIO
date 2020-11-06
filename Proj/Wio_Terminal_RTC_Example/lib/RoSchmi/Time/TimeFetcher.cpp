#include <Arduino.h>
#include <Time/SysTime.h>
#include <Time/TimeFetcher.h>

static SysTime sysTime;

TimeFetcher::TimeFetcher()
{ }

TimeFetcher::~TimeFetcher(){};

DateTime TimeFetcher::getTime()
{   
    DateTime now = sysTime.getTime();
    return now;
}
