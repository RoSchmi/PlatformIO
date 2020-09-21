#include <Arduino.h>
//#include <AzureStorage/TestPassInteger.h>

#ifndef _TESTPASSINTEGER_H_
#define _TESTPASSINTEGER_H_

class TestPassInteger
{
public:
    TestPassInteger(int pIntergerValue);
    ~TestPassInteger();

    int getValue();
};

#endif 