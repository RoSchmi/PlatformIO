#include <Arduino.h>
#include <AzureStorage/ClassWithMethods.h>

int _input;

ClassWithMethods::ClassWithMethods(int input)
{
    _input = input;

};
    ClassWithMethods::~ClassWithMethods(){}