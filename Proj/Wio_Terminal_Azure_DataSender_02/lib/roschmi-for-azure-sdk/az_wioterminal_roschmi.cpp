// Copyright of template
// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

// Copyright (c) RoSchmi. All rights reserved.
// SPDX-License-Identifier: MIT

#include <azure/core/az_http.h>
#include <azure/core/az_http_transport.h>
#include <azure/core/az_span.h>
#include <azure/core/internal/az_result_internal.h>
#include <azure/core/internal/az_span_internal.h>

#include <stdlib.h>

#include <Arduino.h>
#include <string.h>

#include <azure/core/az_platform.h>

#include <az_wioterminal_roschmi.h>

/*
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
*/


HTTPClient *  deviceHttp = NULL;

const char * _caCertificate;


/**
 * @brief uses AZ_HTTP_BUILDER to set up CURL request and perform it.
 *
 * @param request an internal http builder with data to build and send http request
 * @param ref_response pre-allocated buffer where http response will be written
 * @return az_result
 */
AZ_NODISCARD az_result
az_http_client_send_request(az_http_request const* request, az_http_response* ref_response)
{
  _az_PRECONDITION_NOT_NULL(request);
  _az_PRECONDITION_NOT_NULL(ref_response);

// Working with spans
//https://github.com/Azure/azure-sdk-for-c/tree/master/sdk/docs/core#working-with-spans
  
  az_context* requContext = request->_internal.context;
  az_http_method requMethod = request->_internal.method;
  //az_span requUrl = request->_internal.url;
  az_span requUrl = az_span_slice(request->_internal.url, 0, 100);
  
  

  char requUrlStr [101];
  az_span_to_str(requUrlStr, 101, requUrl);
  String requUrlString = String(requUrlStr);

    String theUrl = "jsonplaceholder.typicode.com";
    String thePath = "/posts?userId=1";
    
    deviceHttp->begin(theUrl, (uint16_t)443, thePath, _caCertificate);
    int httpCode = deviceHttp->GET();
    if (httpCode > 0) { //Check for the returning code 
      String payload = deviceHttp->getString();
      //lcd_log_line("Http-Code:");
      //lcd_log_line(itoa((int)httpCode, buf, 10));
      int length = payload.length();
      int indexCtr = 0;
      int pageWidth = 30;
      Serial.println(httpCode);
      delay(2000);
      while (indexCtr < length)
      {
        indexCtr += pageWidth;
      } 
       Serial.println(payload);
    }
    else {
     // Serial.println("Error on HTTP request");
      volatile int dummy2 = 1;
      dummy2++;
    }



  //_httpPtr->begin(requUrlStr, 443, "/", _caCert);



  volatile int32_t queryStart = request->_internal.query_start;
  
  //String respString = "HTTP/1.1 200 OK";
  char *  responseString = (char*)"HTTP/1.1 200 OK\r\nContent-length:46\r\n\r\n<html>\r\n";

  int32_t theLength = strlen(responseString);

  az_http_response myResponse;
  myResponse._internal.http_response = az_span_create_from_str(responseString);
  
  myResponse._internal.written = 46;
   ref_response = &myResponse;
   return AZ_OK;
}

// AZ_NODISCARD int64_t az_platform_clock_msec() { return 0; }

// For Arduino (Wio Terminal)
// this defines clockCyclesPerMicrosecond if its not already defined
AZ_NODISCARD int64_t az_platform_clock_msec() 
{
  //#ifndef clockCyclesPerMicrosecond()
  //  #define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
  //#endif
  
   return (int64_t)((int64_t)clockCyclesPerMicrosecond() * 1000L); 

}


//void az_platform_sleep_msec(int32_t milliseconds) { (void)milliseconds; }
// For Arduino (Wio Terminal):
void az_platform_sleep_msec(int32_t milliseconds) 
{ 
  delay(milliseconds); 
}

void setHttpClient(HTTPClient * httpClient)
{
  if (deviceHttp == NULL)
  {
      deviceHttp = httpClient;
  }
}

void setCaCert(const char * caCert)
{
  _caCertificate = caCert;
}

