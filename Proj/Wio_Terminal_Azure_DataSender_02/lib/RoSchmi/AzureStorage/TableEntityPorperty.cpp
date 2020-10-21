#include <Arduino.h>
#include <AzureStorage/TableEntityProperty.h>
#include <azure/core/az_span.h>



EntityProperty TableEntityProperty(char * pName, char * pValue, char * pType)
{
    EntityProperty property;

    /*
    char buf[strlen(pName) + strlen(pValue) + strlen(pType) + 25]; 
    //sprintf(buf, "<d:%s m:type=\"%s\">%s</d:%s>", Name, Type, Value, Type, Name);
    sprintf(buf, "<d:%s m:type=%c%s%c>%s</d:%s>", pName, '"', pType, '"', pValue, pType, pName);
    property.Prefix = az_span_create_from_str((char *)buf);
    property.Name = az_span_create_from_str(pName);
    property.Value = az_span_create_from_str(pValue);
    property.Type = az_span_create_from_str(pType);
    return property;
    */
    char buf[strlen(pName) + strlen(pValue) + strlen(pType) + 25]; 
    //sprintf(buf, "<d:%s m:type=\"%s\">%s</d:%s>", Name, Type, Value, Type, Name);
    //sprintf(buf, "<d:%s m:type=%c%s%c>%s</d:%s>", pName, '"', pType, '"', pValue, pType, pName);
    sprintf(buf, "<d:%s m:type=%c%s%c>%s</d:%s>", pName, '"', pType, '"', pValue, pName);
    strcpy(property.Prefix, buf);
    strcpy(property.Name, pName);
    strcpy(property.Value, pValue);
    strcpy(property.Type, pType); 
    return property;
}
