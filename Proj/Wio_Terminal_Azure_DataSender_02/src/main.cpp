#include <Arduino.h>
#include <config.h>
#include <config_secret.h>

#include "az_iot_helpers.h"

#include <WiFiClientSecure.h>

#include "WiFiUdp.h"
#include "NTP.h"

//az_config_internal 
//az_platform

#include <azure/core/az_platform.h>
//#include <platform.h>
#include <azure/core/az_config_internal.h>
#include <azure/core/az_context.h>
#include <azure/storage/az_storage_blobs.h>

//#include <curl/curl.h> // libcurl is our HTTP stack


#include <az_wioterminal_roschmi.h> 

#include <stdio.h>
#include <stdlib.h>


#include "HTTPClient.h"

// Here is the link to the used HTTPClient
// It was added manually to the project
// https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPClient


#include "mbedtls/md.h"
#include "mbedtls/base64.h"
#include "mbedtls/sha256.h"

#include "TFT_eSPI.h"
#include "Free_Fonts.h" //include the header file

#include <azure/core/az_json.h>

#include <azure/core/az_result.h>
#include <azure/core/az_span.h>
#include <azure/core/az_config.h>

#include <azure/core/az_http.h>
#include <azure/core/az_context.h>

#include <azure/core/az_http_transport.h>

//#include <azure/core/_az_cfg_prefix.h>   // may not be included


#include <azure/iot/az_iot_hub_client.h>
//#include "azure/iot/az_iot_provisioning_client.h"


#include <azure/iot/az_iot_common.h>

#include <azure/core/az_platform.h>
#include <azure/core/az_retry_internal.h>

#include <azure/storage/az_storage_blobs.h>

#include <azure/iot/az_iot_hub_client.h>

#include <roschmi_az_http_helpers.h>

TFT_eSPI tft;
int current_text_line = 0;

#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define LCD_FONT FreeSans9pt7b
#define LCD_LINE_HEIGHT 18

#define TIMEZONE 1.0      // TimeZone time difference to UTC

#define URI_ENV "AZURE_STORAGE_URL"

volatile int counter = 0;
volatile int counter2 = 0;
char strData[100];

const char *ssid = IOT_CONFIG_WIFI_SSID;
const char *password = IOT_CONFIG_WIFI_PASSWORD;



/*
const char *baltimore_root_ca =
"-----BEGIN CERTIFICATE-----\n"
"MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"
"RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"
"VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"
"DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"
"ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"
"VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"
"mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"
"IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"
"mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"
"XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"
"dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"
"jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"
"BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"
"DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"
"9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"
"jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"
"Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"
"ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"
"R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"
"-----END CERTIFICATE-----";
*/

WiFiClientSecure wifi_client;

WiFiUDP wifiUdp;
NTP ntp(wifiUdp);
HTTPClient http;

void lcd_log_line(char* line) {
    // clear line
    tft.fillRect(0, current_text_line * LCD_LINE_HEIGHT, 320, LCD_LINE_HEIGHT, TFT_WHITE);
    tft.drawString(line, 5, current_text_line * LCD_LINE_HEIGHT);

    current_text_line++;
    current_text_line %= ((LCD_HEIGHT-20)/LCD_LINE_HEIGHT);
    if (current_text_line == 0) {
      tft.fillScreen(TFT_WHITE);
    }
}
void setup() {
   tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);
  tft.setFreeFont(&LCD_FONT);
  tft.setTextColor(TFT_BLACK);

  pinMode(LED_BUILTIN, OUTPUT);
  lcd_log_line((char *)"Starting");
  
  Serial.begin(9600);
  Serial.println("\r\nHello, I'm starting ");

  char buf[42];
    sprintf(buf, "Connecting to SSID: %s", ssid);
    lcd_log_line(buf);
    Serial.println(buf);
    WiFi.enableIpV6();
    WiFi.begin(ssid, password);

    // attempt to connect to Wifi network:
        
    while((WiFi.status() != WL_CONNECTED))
    {  
      lcd_log_line(itoa((int)WiFi.status(), buf, 10));
        Serial.print(".");
        // wait 1 second for re-trying
        delay(1000);        
    }
    IPAddress localIpAddress = WiFi.localIP();
    IPAddress gatewayIp =  WiFi.gatewayIP();
    IPAddress subNetMask =  WiFi.subnetMask();
    IPAddress dnsServerIp = WiFi.dnsIP();
    
    lcd_log_line((char *)"> SUCCESS.");
    lcd_log_line((char*)localIpAddress.toString().c_str() );
    lcd_log_line((char*)gatewayIp.toString().c_str());
    lcd_log_line((char*)subNetMask.toString().c_str());
    lcd_log_line((char*)dnsServerIp.toString().c_str());

    Serial.println("\r\n> SUCCESS.");

    wifi_client.setCACert(baltimore_root_ca);

    
    
    ntp.begin();
    ntp.update();
    ntp.timeZone(1,0);
    
    lcd_log_line((char *)ntp.formattedTime("%d. %B %Y"));    // dd. Mmm yyyy
    lcd_log_line((char *)ntp.formattedTime("%A %T"));        // Www hh:mm:ss

// Example see  
// https://github.com/Azure/azure-sdk-for-c/blob/5c7444dfcd5f0b3bcf3aec2f7b62639afc8bd664/sdk/samples/storage/blobs/src/blobs_client_example.c

//Für ein BLOB schließt der Basis-URI den Namen des Kontos, den Namen des Containers und den Namen des BLOB ein:
//https://myaccount.blob.core.windows.net/mycontainer/myblob
// https://docs.microsoft.com/de-de/rest/api/storageservices/naming-and-referencing-containers--blobs--and-metadata

    az_storage_blobs_blob_client client;

    az_storage_blobs_blob_client_options options = az_storage_blobs_blob_client_options_default();
    
    //char* thisEnv = getenv(URI_ENV);
    char* thisUrl = (char *)"https://roschmi01.blob.core.windows.net/con7/myblob/";

    //volatile String thisEnvStr(thisUrl);
    
      if (az_storage_blobs_blob_client_init(
          &client, az_span_create_from_str(thisUrl), AZ_CREDENTIAL_ANONYMOUS, &options)
      != AZ_OK)
  {
    lcd_log_line((char *)"Client not created");
    //printf("\nFailed to init blob client\n");
    //return 1;
  }
  az_span theEndpoint = client._internal.endpoint;

   volatile uint32_t theLenght = theEndpoint._internal.size;
 
  // Prepare response
  //uint8_t response_buffer[1024 * 4] = { 0 };
  uint8_t response_buffer[1024 * 1] = { 0 };      // may not be > 2048 otherwise http Client crashes
  az_http_response http_response;
  
  if (az_http_response_init(&http_response, AZ_SPAN_FROM_BUFFER(response_buffer)) != AZ_OK)
  {
    lcd_log_line((char *)"Failed http response");
  }

delay(1000);
    //current_text_line = 0;
    //tft.fillScreen(TFT_WHITE);
delay(100);
    
    http.begin(wifi_client, "jsonplaceholder.typicode.com", 443, "/posts?userId=1", true);
    //http.begin(wifi_client, "www.google.de", 443, "/", true);
    delay(100);
    int httpCode = http.GET();
    delay(100);
    if (httpCode > 0) { //Check for the returning code 
      String payload = http.getString();
      lcd_log_line("Http-Code:");
      lcd_log_line(itoa((int)httpCode, buf, 10));
      int length = payload.length();
      int indexCtr = 0;
      int pageWidth = 30;
      Serial.println(httpCode);
      delay(2000);
      while (indexCtr < length)
      {
        lcd_log_line((char*)payload.substring(indexCtr, indexCtr + pageWidth).c_str());
        indexCtr += pageWidth;
      } 
      Serial.println(payload);
    }
    else {
      Serial.println("Error on HTTP request");
      volatile int dummy2 = 1;
      dummy2++;
    }

  /*
  //String respString = "HTTP/1.1 200 OK";
  char *  responseString = (char*)"HTTP/1.1 200 OK\r\nContent-length:46\r\n\r\n<html>\r\n";
  http_response._internal.http_response = az_span_create_from_str(responseString);
  http_response._internal.written = 46;

   static az_span content_to_upload = AZ_SPAN_LITERAL_FROM_STR("Some test content");
  
  az_result const blob_upload_result
      = az_storage_blobs_blob_upload(&client, content_to_upload, NULL, &http_response);

// This validation is only for the first time SDK client is used. API will return not implemented
  // if samples were built with no_http lib.

if (blob_upload_result == AZ_ERROR_NOT_IMPLEMENTED)
  {
    lcd_log_line((char *)"Failed, no http");
    //printf("Running sample with no_op HTTP implementation.\nRecompile az_core with an HTTP client "
    //      "implementation like CURL to see sample sending network requests.\n\n"
    //       "i.e. cmake -DTRANSPORT_CURL=ON ..\n\n");

    //return 1;
  }
  else
  {
    if (blob_upload_result != AZ_OK) // Any other error would terminate sample
    {
      delay(2000);
      current_text_line = 0;
      lcd_log_line((char *)"Failed, other");
      delay(4000);
      //printf("\nFailed to upload blob\n");
      //return 1;
    }
    else
    {
      
    }
  }
  */
}


void loop() {
   //delay(200);


  for (int i = 0; i < 5; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    
    delay(500);
    sprintf(strData, "Inner Loop %u", i);   
    lcd_log_line(strData);
    //Serial.print("Inner Loop ");
    //Serial.println(i);
  }
    sprintf(strData, "   Outer Loop %u", counter);   
    lcd_log_line(strData);
    //Serial.print("    Outer Loop ");
    //Serial.println(counter);
    counter++;
}



