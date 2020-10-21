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
  
  
  az_http_method requMethod = request->_internal.method;
  //az_span requUrl = request->_internal.url;
  
  volatile int32_t max_header_count = request->_internal.max_headers;

  volatile int32_t theQueryStart = request->_internal.query_start;
  volatile size_t headerCount = az_http_request_headers_count(request);
  /*
  az_span header_name = { 0 };
  az_span header_value = { 0 };
  az_http_request_get_header(request, 1, &header_name, &header_value);
  */

  /*
  char requUrlStr [101];
  az_span_to_str(requUrlStr, 101, requUrl);
  String requUrlString = String(requUrlStr);
  */

uint8_t headers_buffer[300] {0};
  az_span headers_span = AZ_SPAN_FROM_BUFFER(headers_buffer);
  //az_span separator = AZ_SPAN_LITERAL_FROM_STR("**");




az_result myResult = dev_az_http_client_build_headers(request, headers_span);

char* theHeader_str = (char*) az_span_ptr(headers_span);
volatile size_t headerSize = strlen(theHeader_str);
   
  /* 
  // String theUrl =  (char *)az_span_ptr(requUrl); 
  // String thePath = "/";  

   String theUrl = "jsonplaceholder.typicode.com";
   String thePath = "/posts?userId=1";       
    
    //deviceHttp->begin((char *)az_span_ptr(requUrl), (uint16_t)443, "/", _caCertificate);
    //deviceHttp->begin((char *)az_span_ptr(requUrl), (uint16_t)80, "/", _caCertificate);
    deviceHttp->begin(theUrl, (uint16_t)443, thePath, _caCertificate);
    int httpCode = deviceHttp->GET();
    delay(100);
    //((char *)az_span_ptr( request->_internal.body));
    if (httpCode > 0) { //Check for the returning code 
      String payload = deviceHttp->getString();
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
    */

    
    //String theUrl = "jsonplaceholder.typicode.com";
    //String thePath = "/posts?userId=1";

    

    //az_span requUrl = az_span_slice(request->_internal.url, 0, 100);
    char urlBuffer[az_span_size(request->_internal.url) + 2] {0};
    az_span requUrl = AZ_SPAN_FROM_BUFFER(urlBuffer);

    az_span_copy(requUrl, request->_internal.url);
    //String theUrl = "prax47.table.core.windows.net";

    String urlString = (char *)urlBuffer;
    int foundProtocol = urlString.indexOf(':');
    String protocol = "";
    if (foundProtocol != -1)
    {
        protocol = urlString.substring(0, foundProtocol);
        if ((protocol == "https") || (protocol == "http"))
        {
          urlString = urlString.substring(foundProtocol + 3);
        }       
    }
    int foundSlash = urlString.indexOf('/');
    String host = foundSlash != -1 ? urlString.substring(0, foundSlash) : urlString;
    String resource = foundSlash != -1 ? urlString.substring(foundSlash) : "";
    uint16_t port = protocol == "http" ? 80 : 443;
    
    if (port == 80)
    {
      deviceHttp->begin(host, port, resource);
    }
    else
    {
      deviceHttp->begin(host, port, resource, _caCertificate);
    }
    deviceHttp->setReuse(false);
    deviceHttp->setUserAgent("RsNetmfHttpClient");
    
    
    //deviceHttp->begin(theUrl, (uint16_t)80, thePath);
    //int httpCode = deviceHttp->GET();
    //deviceHttp->setUserAgent("RsWioClient");
    
    //deviceHttp->addHeader()


    size_t valueBufferLength = request->_internal.url_length + 40;
    char name_buffer[30] {0};
    char value_buffer[valueBufferLength] {0};
    az_span head_name = AZ_SPAN_FROM_BUFFER(name_buffer);
    az_span head_value = AZ_SPAN_FROM_BUFFER(value_buffer);
    
    String nameString = "";
    String valueString = "";

    for (int32_t offset = (headerCount - 1); offset >= 0; offset--)
    {
      _az_RETURN_IF_FAILED(az_http_request_get_header(request, offset, &head_name, &head_value));
      
      az_span_to_str((char *)name_buffer, 30, head_name);
      az_span_to_str((char *)value_buffer, valueBufferLength, head_value);
      nameString = (char *)name_buffer;
      valueString = (char *)value_buffer;

      deviceHttp->addHeader(nameString, valueString, true, true);   
    }
    //request->_internal.body

    
    
    
    //size_t bodyLength = az_span_size(request->_internal.body);

    //az_span body = request->_internal.body;

    //size_t bodyLength = az_span_size(body);

    int32_t bodySize = request->_internal.body._internal.size;
    char theBody[bodySize + 10] {0};
    
    az_span_to_str((char *)theBody, bodySize + 1, request->_internal.body);
    //theBody[bodySize] = '\r';
    //theBody[bodySize + 1] = '\n';



    String bodyString = (char *)theBody;

    String bodyString_01 = bodyString.substring(0, 100);
    String bodyString_02 = bodyString.substring(100, 200);
    String bodyString_03 = bodyString.substring(200, 300);
    String bodyString_04 = bodyString.substring(300, 400);
    String bodyString_05 = bodyString.substring(400, 500);
    String bodyString_06 = bodyString.substring(500, 600);
    String bodyString_07 = bodyString.substring(600, 700);
    String bodyString_08 = bodyString.substring(700, 800);
    String bodyString_09 = bodyString.substring(800);







    volatile size_t thebodiesLength = strlen(theBody);
    volatile char byte_00 = bodyString[0];
    volatile char byte_821 = bodyString[821];
    volatile char byte_822 = bodyString[822];
    volatile char byte_823 = bodyString[823];
    volatile char byte_824 = bodyString[824];



  
    if (az_span_is_content_equal(requMethod, AZ_SPAN_LITERAL_FROM_STR("POST")))
    //if (requMethod az_span_c  az_http_method_post )
    {
      int httpCode = deviceHttp->POST(bodyString);
      delay(110);
      if (httpCode > 0) { //Check for the returning code 
        String payload = deviceHttp->getString();
        //lcd_log_line("Http-Code:");
        //lcd_log_line(itoa((int)httpCode, buf, 10));
        int length = payload.length();
        int indexCtr = 0;
        int pageWidth = 50;
        Serial.println(httpCode);
        delay(2000);
        String partMessage;
        while (indexCtr < length)
        {
          partMessage = payload.substring(indexCtr, indexCtr + pageWidth);
          indexCtr += pageWidth;
        } 
        Serial.println(payload);
      }
      else {
        // Serial.println("Error on HTTP request");
        volatile int dummy2 = 1;
        dummy2++;
      }
    }
    else
    {
      if (az_span_is_content_equal(requMethod, AZ_SPAN_LITERAL_FROM_STR("GET")))
      {
        volatile int dummy7jsgj = 1;
      }
      else
      {
        volatile int dummy7jdgj = 1;
      }
      
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

/**
 * @brief loop all the headers from a HTTP request and compine all headers into one az_span
 *
 * @param request an http builder request reference
 * @param ref_headers list of headers
 * @return az_result
 */

static AZ_NODISCARD az_result
dev_az_http_client_build_headers(az_http_request const* request, az_span ref_headers)
{
  _az_PRECONDITION_NOT_NULL(request);
  
  az_span header_name = { 0 };
  az_span header_value = { 0 };
  uint8_t header_buffer[request->_internal.url_length - 20 + 60] {0};
  az_span header_span = AZ_SPAN_FROM_BUFFER(header_buffer);
  az_span separator = AZ_SPAN_LITERAL_FROM_STR(": ");
  

  for (int32_t offset = 0; offset < az_http_request_headers_count(request); ++offset)
  {
    _az_RETURN_IF_FAILED(az_http_request_get_header(request, offset, &header_name, &header_value));   
    _az_RETURN_IF_FAILED(dev_az_span_append_header_to_buffer(header_span, header_name, header_value, separator));
    char* header_str = (char*) az_span_ptr(header_span); // str points to a 0-terminated string
    ref_headers = az_span_copy(ref_headers, az_span_create_from_str(header_str));
    ref_headers = az_span_copy(ref_headers, AZ_SPAN_LITERAL_FROM_STR("\r\n"));
  }
  az_span_copy_u8(ref_headers, 0);
  return AZ_OK;
}


/**
 * @brief writes a header key and value to a buffer as a 0-terminated string and using a separator
 * span in between. Returns error as soon as any of the write operations fails
 *
 * @param writable_buffer pre allocated buffer that will be used to hold header key and value
 * @param header_name header name
 * @param header_value header value
 * @param separator symbol to be used between key and value
 * @return az_result
 */
static AZ_NODISCARD az_result dev_az_span_append_header_to_buffer(
    az_span writable_buffer,
    az_span header_name,
    az_span header_value,
    az_span separator)
{
  int32_t required_length
      = az_span_size(header_name) + az_span_size(separator) + az_span_size(header_value) + 1;

  _az_RETURN_IF_NOT_ENOUGH_SIZE(writable_buffer, required_length);

  writable_buffer = az_span_copy(writable_buffer, header_name);
  writable_buffer = az_span_copy(writable_buffer, separator);
  writable_buffer = az_span_copy(writable_buffer, header_value);
  az_span_copy_u8(writable_buffer, 0);
  return AZ_OK;
}
/*
volatile byte c0 = headers_buffer[0];
              volatile byte c1 = headers_buffer[1];
              volatile byte c2 = headers_buffer[2];
              volatile byte c3 = headers_buffer[3];
              volatile byte c4 = headers_buffer[4];
              volatile byte c5 = headers_buffer[5];
              volatile byte c6 = headers_buffer[6];
              volatile byte c7 = headers_buffer[7];
              volatile byte c8 = headers_buffer[8];
              volatile byte c9 = headers_buffer[9];
              volatile byte c10 = headers_buffer[10];
              volatile byte c11 = headers_buffer[11];
              volatile byte c12 = headers_buffer[12];
              volatile byte c13 = headers_buffer[13];
              volatile byte c14 = headers_buffer[14];
              volatile byte c15 = headers_buffer[15];
              volatile byte c16 = headers_buffer[16];
              volatile byte c17 = headers_buffer[17];
              volatile byte c18 = headers_buffer[18];
              volatile byte c19 = headers_buffer[19];
              volatile byte c20 = headers_buffer[20];
              volatile byte c21 = headers_buffer[21];
              volatile byte c22 = headers_buffer[22];
              volatile byte c23 = headers_buffer[23];
              volatile byte c24 = headers_buffer[24];
              volatile byte c54 = headers_buffer[25];
              volatile byte c55 = headers_buffer[26];
              volatile byte c56 = headers_buffer[27];
              volatile byte c57 = headers_buffer[28];
              volatile byte c58 = headers_buffer[29];
              volatile byte c59 = headers_buffer[30];
              volatile byte c60 = headers_buffer[31];
              volatile byte c61 = headers_buffer[32];
              volatile byte c62 = headers_buffer[33];
              volatile byte c63 = headers_buffer[34];
              volatile byte c64 = headers_buffer[35];
              */