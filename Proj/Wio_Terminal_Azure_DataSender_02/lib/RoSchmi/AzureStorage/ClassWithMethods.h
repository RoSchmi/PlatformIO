#include <Arduino.h>

#ifndef _CLASSWITHMETHODS_H_
#define _CLASSWITHMETHODS_H_

class ClassWithMethods
{
public:
    ClassWithMethods(int input);
    ~ClassWithMethods();
    
    int multiply();
};
#endif 