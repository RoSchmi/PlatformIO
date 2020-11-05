#include <Arduino.h>
#include <config.h>
#include <config_secret.h>

#include <AzureStorage/CloudStorageAccount.h>
#include <AzureStorage/TableClient.h>
#include <AzureStorage/TableEntityProperty.h>
#include <AzureStorage/TableEntity.h>
#include <AzureStorage/AnalogTableEntity.h>

#include "az_iot_helpers.h"

#include <WiFiClientSecure.h>

#include "WiFiUdp.h"
#include "NTP.h"
//#include "RTC_SAMD51.h"
#include "DateTime.h"
#include <time.h>
#include <Time.h>

#include <Time/SysTime.h>

#include <azure/core/az_platform.h>
//#include <platform.h>
#include <azure/core/az_config_internal.h>
#include <azure/core/az_context.h>
#include <azure/core/az_http.h>


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

#include <azure/iot/az_iot_common.h>

#include <azure/core/az_platform.h>
#include <azure/core/az_retry_internal.h>

#include <azure/iot/az_iot_hub_client.h>

#include <roschmi_az_http_helpers.h>

// The PartitionKey may have a prefix to be distinguished, here: "Y2_" 
const char * analogTablePartPrefix = (char *)"Y2_";

// The PartitinKey can be augmented with a string representing year and month (recommended)
const bool augmentPartitionKey = true;

// The TableName can be augmented with the actual year (recommended)
const bool augmentTableNameWithYear = true;

TFT_eSPI tft;
int current_text_line = 0;

#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define LCD_FONT FreeSans9pt7b
#define LCD_LINE_HEIGHT 18

#define TIMEZONE 60     // TimeZone time difference to UTC in minutes
#define DSTOFFSET 60    // DaylightSaving Time offset in minutes

#define URI_ENV "AZURE_STORAGE_URL"



volatile int counter = 0;
volatile int counter2 = 0;
char strData[100];

const char *ssid = IOT_CONFIG_WIFI_SSID;
const char *password = IOT_CONFIG_WIFI_PASSWORD;

WiFiClientSecure wifi_client;

WiFiUDP wifiUdp;
NTP ntp(wifiUdp);

HTTPClient http;
HTTPClient * httpPtr = &http;

// must be static !!
static SysTime sysTime;

typedef const char* X509Certificate;

X509Certificate myX509Certificate = baltimore_root_ca;

CloudStorageAccount myCloudStorageAccount(AZURE_CONFIG_ACCOUNT_NAME, AZURE_CONFIG_ACCOUNT_KEY, false);
CloudStorageAccount * myCloudStorageAccountPtr = &myCloudStorageAccount;


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



// forward declarations
void createSampleTime(DateTime dateTimeUTCNow, int timeZoneOffsetUTC, char * sampleTime);
az_http_status_code  createTable(CloudStorageAccount * myCloudStorageAccountPtr, X509Certificate myX509Certificate, const char * tableName);
az_http_status_code CreateTable( const char * tableName, ContType pContentType, AcceptType pAcceptType, ResponseType pResponseType, bool);
az_http_status_code insertTableEntity(CloudStorageAccount *myCloudStorageAccountPtr, X509Certificate pCaCert, const char * pTableName, TableEntity pTableEntity, char * outInsertETag);
//az_http_status_code insertTableEntity(CloudStorageAccount *pAccountPtr, X509Certificate pCaCert, const char * pTableName);
void makePartitionKey(const char * partitionKeyprefix, bool augmentWithYear, az_span outSpan, size_t *outSpanLength);
void makeRowKey(DateTime actDate, az_span outSpan, size_t *outSpanLength);
//AnalogTableEntity analogTableEntity(az_span partitionKey, az_span rowKey, EntityProperty AnalogPropertiesArray[]);

//az_http_status_code CreateTable(const char * tableName, ContType pContentType = ContType::contApplicationIatomIxml, AcceptType pAcceptType = AcceptType::acceptApplicationIjson, ResponseType pResponseType = ResponseType::returnContent, bool useSharedKeyLight = false);

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

  //pinMode(BUTTON_1, INPUT_PULLUP);
  
  /*
  while(digitalRead(BUTTON_1) == HIGH)
  {
    delay(100);
    delay(100);
    delay(100);
  }
  */
  

  delay(5000);
  lcd_log_line((char *)"Continue");

  char buf[42];
    sprintf(buf, "Connecting to SSID: %s", ssid);
    lcd_log_line(buf);    
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
   
    wifi_client.setCACert(baltimore_root_ca);

    ntp.begin();
    ntp.update();
    // Set Daylightsavingtime for central europe
    ntp.ruleDST("CEST", Last, Sun, Mar, 2, 120); // last sunday in march 2:00, timetone +120min (+1 GMT + 1h summertime offset)
    ntp.ruleSTD("CET", Last, Sun, Oct, 3, 60); // last sunday in october 3:00, timezone +60min (+1 GMT)
    ntp.updateInterval(3000000);  // Update every 30 minutes
    
    
    lcd_log_line((char *)ntp.formattedTime("%d. %B %Y"));    // dd. Mmm yyyy
    lcd_log_line((char *)ntp.formattedTime("%A %T"));        // Www hh:mm:ss

    //DateTime now = DateTime(F(__DATE__), F(__TIME__));
    //DateTime now = DateTime(F((char *)ntp.formattedTime("%d. %B %Y")), F((char *)ntp.formattedTime("%A %T")));
    DateTime dateTimeUTCNow = DateTime((uint16_t) ntp.year(), (uint8_t)ntp.month(), (uint8_t)ntp.day(),
                (uint8_t)ntp.hours(), (uint8_t)ntp.minutes(), (uint8_t)ntp.seconds());

    // Set rtc to UTC Time
    sysTime.begin(dateTimeUTCNow);
    
    
    
    //String myUriEndPoint =  myCloudStorageAccount.UriEndPointTable;

// Example see 
// https://github.com/Azure/azure-sdk-for-c/blob/5c7444dfcd5f0b3bcf3aec2f7b62639afc8bd664/sdk/samples/storage/blobs/src/blobs_client_example.c

//Für ein BLOB schließt der Basis-URI den Namen des Kontos, den Namen des Containers und den Namen des BLOB ein:
//https://myaccount.blob.core.windows.net/mycontainer/myblob
// https://docs.microsoft.com/de-de/rest/api/storageservices/naming-and-referencing-containers--blobs--and-metadata



 
  // Prepare response
  //uint8_t response_buffer[1024 * 4] = { 0 };

  /*
  uint8_t response_buffer[1024 * 1] = { 0 };      // may not be > 2048 otherwise http Client crashes
  az_http_response http_response;
  
  if (az_http_response_init(&http_response, AZ_SPAN_FROM_BUFFER(response_buffer)) != AZ_OK)
  {
    lcd_log_line((char *)"Failed http response");
  }
  */
delay(1000);
    current_text_line = 0;
    tft.fillScreen(TFT_WHITE);
delay(100);

TableClient table(myCloudStorageAccountPtr, myX509Certificate, httpPtr);

String tableName = "AnalogTestValues";
if (augmentTableNameWithYear)
{
  tableName += (dateTimeUTCNow.year() - 30);
}


//const char * tableName = "AnalogTestValues2020";

// RoSchmi: do not delete
//az_http_status_code theResult = createTable(myCloudStorageAccountPtr, myX509Certificate, (char *)tableName.c_str());

char * sampleValue_1 = (char *)"17.1";
char * sampleValue_2 = (char *)"17.2";
char * sampleValue_3 = (char *)"17.3";
char * sampleValue_4 = (char *)"17.4";



int timeZoneOffsetUTC = ntp.isDST() ? TIMEZONE + DSTOFFSET : TIMEZONE;
//char sign = timeZoneOffsetUTC < 0 ? '-' : '+';
//char TimeOffsetUTCString[10];
//sprintf(TimeOffsetUTCString, " %c%03i", sign, timeZoneOffsetUTC);

dateTimeUTCNow = sysTime.getTime();

char sampleTime[25] {0};
createSampleTime(dateTimeUTCNow, timeZoneOffsetUTC, (char *)sampleTime);

//sprintf(sampleTime, "%02i/%02i/%04i %02i:%02i:%02i%s",dateTimeUTCNow.month(), dateTimeUTCNow.day(), dateTimeUTCNow.year() - 30, dateTimeUTCNow.hour(), dateTimeUTCNow.minute(), dateTimeUTCNow.second(), TimeOffsetUTCString);

//TimeSpan timeSpanOffsetToUTC = TimeSpan(0,0,)
//DateTime localNow = dateTimeUTCNow - 



   uint32_t * ptr_one;
   uint32_t * last_ptr_one;
   // This code snippet was used to get the addresses of the heap
   
   /*
   for (volatile int i = 0; 1 < 100000; i++)
   {
     last_ptr_one = ptr_one;
     ptr_one = 0;
     ptr_one = (uint32_t *)malloc(1);
     if (ptr_one == 0)
     {
       ptr_one = last_ptr_one;
       volatile int dummy68424 = 1;
     }
     else
     {
       *ptr_one = (uint32_t)0xAA55AA55;
       char printBuf[25];
       
       if (((uint32_t)ptr_one % 256)== 0)
       {
         sprintf(printBuf, "%09x", (uint32_t)ptr_one);
          lcd_log_line((char*)printBuf);
       }
     } 
   }
   */
   
  // Fills heap from 0x20029000 - 0x2002FE00 with pattern AA55
  // So you can see at breakpoints how much of heap was used

  ptr_one = (uint32_t *)0x20028F80;
  while (ptr_one < (uint32_t *)0x2002fe00)
  {
    *ptr_one = (uint32_t)0xAA55AA55;
     ptr_one++;
  }
  
  size_t propertyCount = 5;
  EntityProperty AnalogPropertiesArray[5];
  
  AnalogPropertiesArray[0] = (EntityProperty)TableEntityProperty((char *)"SampleTime", (char *) sampleTime, (char *)"Edm.String");
  AnalogPropertiesArray[1] = (EntityProperty)TableEntityProperty((char *)"T_1", sampleValue_1, (char *)"Edm.String");
  AnalogPropertiesArray[2] = (EntityProperty)TableEntityProperty((char *)"T_2", sampleValue_2, (char *)"Edm.String");
  AnalogPropertiesArray[3] = (EntityProperty)TableEntityProperty((char *)"T_3", sampleValue_3, (char *)"Edm.String");
  AnalogPropertiesArray[4] = (EntityProperty)TableEntityProperty((char *)"T_4", sampleValue_4, (char *)"Edm.String");
  
  char partKeySpan[25] {0};
  size_t partitionKeyLength = 0;
  az_span partitionKey = AZ_SPAN_FROM_BUFFER(partKeySpan);
  makePartitionKey(analogTablePartPrefix, augmentPartitionKey, partitionKey, &partitionKeyLength);
  partitionKey = az_span_slice(partitionKey, 0, partitionKeyLength);

  char rowKeySpan[25] {0};
  size_t rowKeyLength = 0;
  az_span rowKey = AZ_SPAN_FROM_BUFFER(rowKeySpan);
  makeRowKey(dateTimeUTCNow, rowKey, &rowKeyLength);
  rowKey = az_span_slice(rowKey, 0, rowKeyLength);

  dateTimeUTCNow = sysTime.getTime();
 
  AnalogTableEntity analogTableEntity(partitionKey, rowKey, az_span_create_from_str((char *)sampleTime),  AnalogPropertiesArray, propertyCount);
  char EtagBuffer[5] {0};
  az_http_status_code result3 = insertTableEntity(myCloudStorageAccountPtr, myX509Certificate, (char *)tableName.c_str(), analogTableEntity, (char *)EtagBuffer);

  

  

//TableClient table(myCloudStorageAccountPtr, myX509Certificate, httpPtr);

    /*
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
    */


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

   DateTime dateTimeUTCNow = sysTime.getTime();
   if (ntp.update())     // if update interval has expired
   {
       dateTimeUTCNow = DateTime((uint16_t) ntp.year(), (uint8_t)ntp.month(), (uint8_t)ntp.day(),
                (uint8_t)ntp.hours(), (uint8_t)ntp.minutes(), (uint8_t)ntp.seconds());

    // Set rtc to UTC Time
    sysTime.setTime(dateTimeUTCNow);
   }

TableClient table(myCloudStorageAccountPtr, myX509Certificate, httpPtr);

int timeZoneOffsetUTC = ntp.isDST() ? TIMEZONE + DSTOFFSET : TIMEZONE;
char sampleTime[25] {0};
createSampleTime(dateTimeUTCNow, timeZoneOffsetUTC, (char *)sampleTime);

String tableName = "AnalogTestValues";
if (augmentTableNameWithYear)
{
  tableName += (dateTimeUTCNow.year() - 30);
}
char * sampleValue_1 = (char *)"17.1";
char * sampleValue_2 = (char *)"17.2";
char * sampleValue_3 = (char *)"17.3";
char * sampleValue_4 = (char *)"17.4";


  size_t propertyCount = 5;
  EntityProperty AnalogPropertiesArray[5];
  AnalogPropertiesArray[0] = (EntityProperty)TableEntityProperty((char *)"SampleTime", (char *) sampleTime, (char *)"Edm.String");
  AnalogPropertiesArray[1] = (EntityProperty)TableEntityProperty((char *)"T_1", sampleValue_1, (char *)"Edm.String");
  AnalogPropertiesArray[2] = (EntityProperty)TableEntityProperty((char *)"T_2", sampleValue_2, (char *)"Edm.String");
  AnalogPropertiesArray[3] = (EntityProperty)TableEntityProperty((char *)"T_3", sampleValue_3, (char *)"Edm.String");
  AnalogPropertiesArray[4] = (EntityProperty)TableEntityProperty((char *)"T_4", sampleValue_4, (char *)"Edm.String");
  
  char partKeySpan[25] {0};
  size_t partitionKeyLength = 0;
  az_span partitionKey = AZ_SPAN_FROM_BUFFER(partKeySpan);
  makePartitionKey(analogTablePartPrefix, augmentPartitionKey, partitionKey, &partitionKeyLength);
  partitionKey = az_span_slice(partitionKey, 0, partitionKeyLength);
  char rowKeySpan[25] {0};
  size_t rowKeyLength = 0;
  az_span rowKey = AZ_SPAN_FROM_BUFFER(rowKeySpan);
  makeRowKey(dateTimeUTCNow, rowKey, &rowKeyLength);
  rowKey = az_span_slice(rowKey, 0, rowKeyLength);
  
  AnalogTableEntity analogTableEntity(partitionKey, rowKey, az_span_create_from_str((char *)sampleTime),  AnalogPropertiesArray, propertyCount);
  char EtagBuffer[5] {0};
  az_http_status_code result3 = insertTableEntity(myCloudStorageAccountPtr, myX509Certificate, (char *)tableName.c_str(), analogTableEntity, (char *)EtagBuffer);




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

void createSampleTime(DateTime dateTimeUTCNow, int timeZoneOffsetUTC, char * sampleTime)
{
  int hoursOffset = timeZoneOffsetUTC / 60;
  int minutesOffset = timeZoneOffsetUTC % 60;
  char sign = timeZoneOffsetUTC < 0 ? '-' : '+';
  char TimeOffsetUTCString[10];
  sprintf(TimeOffsetUTCString, " %c%03i", sign, timeZoneOffsetUTC);
  TimeSpan timespanOffsetToUTC = TimeSpan(0, hoursOffset, minutesOffset, 0);
  DateTime newDateTime = dateTimeUTCNow + timespanOffsetToUTC;
  sprintf(sampleTime, "%02i/%02i/%04i %02i:%02i:%02i%s",newDateTime.month(), newDateTime.day(), newDateTime.year() - 30, newDateTime.hour(), newDateTime.minute(), newDateTime.second(), TimeOffsetUTCString);
}


 //HttpStatusCode createTable(CloudStorageAccount pCloudStorageAccount, X509Certificate[] pCaCerts, string pTableName)
 
void makeRowKey(DateTime actDate,  az_span outSpan, size_t *outSpanLength)
        {
            // formatting the RowKey (= reverseDate) this way to have the tables sorted with last added row upmost
            char rowKeyBuf[20] {0};
            sprintf(rowKeyBuf, "%4i%02i%02i%02i%02i%02i", (10000 - actDate.year() - 30), (12 - actDate.month()), (31 - actDate.day()), (23 - actDate.hour()), (59 - actDate.minute()), (59 - actDate.second()));
            
            //return (10000 - actDate.Year).ToString("D4") + (12 - actDate.Month).ToString("D2") + (31 - actDate.Day).ToString("D2")
            //           + (23 - actDate.Hour).ToString("D2") + (59 - actDate.Minute).ToString("D2") + (59 - actDate.Second).ToString("D2");
            
            az_span retValue = az_span_create_from_str((char *)rowKeyBuf);
            az_span_copy(outSpan, retValue);
            *outSpanLength = retValue._internal.size;         
        }

void makePartitionKey(const char * partitionKeyprefix, bool augmentWithYear, az_span outSpan, size_t *outSpanLength)
        {
            // if wanted, augment with year and month (12 - month for right order)
            DateTime dateTimeUTCNow = sysTime.getTime();                      
            char dateBuf[20] {0};
            sprintf(dateBuf, "%s%d-%02d", partitionKeyprefix, (dateTimeUTCNow.year() - 30), (12 - dateTimeUTCNow.month()));  
                   
            az_span ret_1 = az_span_create_from_str((char *)dateBuf);
            az_span ret_2 = az_span_create_from_str((char *)partitionKeyprefix);
                       
            if (augmentWithYear == true)
            {
              az_span_copy(outSpan, ret_1);            
              *outSpanLength = ret_1._internal.size; 
            }
            else
            {
              az_span_copy(outSpan, ret_2);
              *outSpanLength = ret_2._internal.size;
            }    
        }

//az_http_status_code insertTableEntity(CloudStorageAccount *pAccountPtr, X509Certificate pCaCert, const char * pTableName)
az_http_status_code insertTableEntity(CloudStorageAccount *pAccountPtr, X509Certificate pCaCert, const char * pTableName, TableEntity pTableEntity, char * outInsertETag)

{
    //HTTPClient http2;
    //HTTPClient * httpPtr2 = &http2;

    TableClient table(pAccountPtr, pCaCert,  httpPtr);

    char codeString[25] {0};
    az_http_status_code statusCode = table.InsertTableEntity(pTableName, pTableEntity,  contApplicationIatomIxml, acceptApplicationIjson, returnContent, false); 
    if ((statusCode == AZ_HTTP_STATUS_CODE_NO_CONTENT) || (statusCode == AZ_HTTP_STATUS_CODE_CREATED))
    { 
      sprintf(codeString, "%s %i", "Entity inserted: ", az_http_status_code(statusCode));    
      lcd_log_line((char *)codeString);
    }
    else
    {
      sprintf(codeString, "%s %i", "Insertion failed: ", az_http_status_code(statusCode));    
      lcd_log_line((char *)codeString);
      delay(10000);
    }
    volatile int dummy7765 = 1;
}



az_http_status_code createTable(CloudStorageAccount *pAccountPtr, X509Certificate pCaCert, const char * pTableName)
{  
  TableClient table(pAccountPtr, pCaCert,  httpPtr);            
  az_http_status_code statusCode = table.CreateTable(pTableName, contApplicationIatomIxml, acceptApplicationIatomIxml, returnContent, false);
  return statusCode;
}



