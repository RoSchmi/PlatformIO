#include <Arduino.h>
#include <azure/core/az_span.h>

#ifndef _TABLE_ENTITY_PROPERTY_H_
#define _TABLE_ENTITY_PROPERTY_H_

/*
typedef struct EntityProperty
{
    az_span Prefix;
    az_span Name;
    az_span Value;
    az_span Type;
};
*/
struct EntityProperty
{
    char Prefix[100];
    char Name[50];
    char Value[50];
    char Type[15];
};

EntityProperty TableEntityProperty(char * Name, char * Value, char * Type);



#endif