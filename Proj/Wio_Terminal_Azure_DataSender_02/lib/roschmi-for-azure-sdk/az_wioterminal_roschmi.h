#include <HTTPClient.h>
#include <Arduino.h>

#ifndef _AZ_WIO_TERMINAL_ROSCHMI_H_
#define _AZ_WIO_TERMINAL_ROSCHMI_H_



void setHttpClient(HTTPClient * httpClient);
void setCaCert(const char * caCert);

#endif