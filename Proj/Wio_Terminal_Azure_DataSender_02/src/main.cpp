#include <Arduino.h>
#include <config.h>
#include <config_secret.h>

#include <AzureStorage/CloudStorageAccount.h>
#include <AzureStorage/TableClient.h>
#include <AzureStorage/TableEntityProperty.h>
#include <AzureStorage/TableEntity.h>
#include <AzureStorage/AnalogTableEntity.h>

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

#include <az_wioterminal_roschmi.h> 

#include <stdio.h>
#include <stdlib.h>

#include "HTTPClient.h"

// Here is the link to the used HTTPClient
// It was added manually to the project
// https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPClient

// in HTTPClient.h comment out the line:: //#include <esp32-hal-log.h>  

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

volatile int counter = 0;
volatile int counter2 = 0;
char strData[100];

const char *ssid = IOT_CONFIG_WIFI_SSID;
const char *password = IOT_CONFIG_WIFI_PASSWORD;

//static bool UseHttps_State = true;

WiFiClientSecure wifi_client;

WiFiUDP wifiUdp;
NTP ntp(wifiUdp);

HTTPClient http;
HTTPClient * httpPtr = &http;

// must be static !!
static SysTime sysTime;

typedef const char* X509Certificate;

X509Certificate myX509Certificate = baltimore_root_ca;

// Set transport protocol as defined in config.h
static bool UseHttps_State = TRANSPORT_PROTOCOL == 0 ? false : true;
CloudStorageAccount myCloudStorageAccount(AZURE_CONFIG_ACCOUNT_NAME, AZURE_CONFIG_ACCOUNT_KEY, UseHttps_State);
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
void makePartitionKey(const char * partitionKeyprefix, bool augmentWithYear, az_span outSpan, size_t *outSpanLength);
void makeRowKey(DateTime actDate, az_span outSpan, size_t *outSpanLength);

void setup() 
{ 
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
  }
  */

  delay(1000);
  
  char buf[42];
  sprintf(buf, "Connecting to SSID: %s", ssid);
  lcd_log_line(buf);    
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:      
  while((WiFi.status() != WL_CONNECTED))
  {  
    lcd_log_line(itoa((int)WiFi.status(), buf, 10));
    // wait 1 second for re-trying
    delay(1000);        
  }
  IPAddress localIpAddress = WiFi.localIP();
  IPAddress gatewayIp =  WiFi.gatewayIP();
  IPAddress subNetMask =  WiFi.subnetMask();
  IPAddress dnsServerIp = WiFi.dnsIP();
  
  
  delay(1000);
  current_text_line = 0;
  tft.fillScreen(TFT_WHITE);
    
  lcd_log_line((char *)"> SUCCESS.");
  sprintf(buf, "Ip: %s", (char*)localIpAddress.toString().c_str());
  lcd_log_line(buf);
  sprintf(buf, "Gateway: %s", (char*)gatewayIp.toString().c_str());
  lcd_log_line(buf);
  sprintf(buf, "Subnet: %s", (char*)subNetMask.toString().c_str());
  lcd_log_line(buf);
  sprintf(buf, "DNS-Server: %s", (char*)dnsServerIp.toString().c_str());
  lcd_log_line(buf);
  sprintf(buf, "Protocol: %s", UseHttps_State ? (char *)"https" : (char *)"http");
  lcd_log_line(buf);
   
  wifi_client.setCACert(baltimore_root_ca);

  ntp.begin();
  ntp.update();

  // Set Daylightsavingtime for central europe
  ntp.ruleDST("CEST", Last, Sun, Mar, 2, 120); // last sunday in march 2:00, timetone +120min (+1 GMT + 1h summertime offset)
  ntp.ruleSTD("CET", Last, Sun, Oct, 3, 60); // last sunday in october 3:00, timezone +60min (+1 GMT)
  ntp.updateInterval(600000);  // Update every 10 minutes
     
  lcd_log_line((char *)ntp.formattedTime("%d. %B %Y"));    // dd. Mmm yyyy
  lcd_log_line((char *)ntp.formattedTime("%A %T"));        // Www hh:mm:ss

  //DateTime now = DateTime(F(__DATE__), F(__TIME__));
  //DateTime now = DateTime(F((char *)ntp.formattedTime("%d. %B %Y")), F((char *)ntp.formattedTime("%A %T")));
  DateTime dateTimeUTCNow = DateTime((uint16_t) ntp.year(), (uint8_t)ntp.month(), (uint8_t)ntp.day(),
                (uint8_t)ntp.hours(), (uint8_t)ntp.minutes(), (uint8_t)ntp.seconds());

  // Set rtc to UTC Time
  sysTime.begin(dateTimeUTCNow);
    
  // I started from Azure Storage Blob Example see: 
  // https://github.com/Azure/azure-sdk-for-c/blob/5c7444dfcd5f0b3bcf3aec2f7b62639afc8bd664/sdk/samples/storage/blobs/src/blobs_client_example.c

  //For a BLOB the Basis-URI consists of the name of the account, the namen of the Container and the namen of the BLOB:
  //https://myaccount.blob.core.windows.net/mycontainer/myblob
  //https://docs.microsoft.com/de-de/rest/api/storageservices/naming-and-referencing-containers--blobs--and-metadata


  delay(5000);
  current_text_line = 0;
  tft.fillScreen(TFT_WHITE);
  

  // This code snippet can be used to get the addresses of the heap
  // and to 
  uint32_t * ptr_one;
  uint32_t * last_ptr_one;
    
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
   
  // Fills memory from 0x20028F80 - 0x2002FE00 with pattern AA55
  // So you can see at breakpoints how much of heap was used
  /*
  ptr_one = (uint32_t *)0x20028F80;
  while (ptr_one < (uint32_t *)0x2002fe00)
  {
    *ptr_one = (uint32_t)0xAA55AA55;
     ptr_one++;
  }
  */

  TableClient table(myCloudStorageAccountPtr, myX509Certificate, httpPtr);

  String tableName = "AnalogTestValues";
  if (augmentTableNameWithYear)
  {
  tableName += (dateTimeUTCNow.year() - 30);
  }

  // RoSchmi: do not delete
  // The following line creates a table in the Azure Storage Account defined in config.h
  az_http_status_code theResult = createTable(myCloudStorageAccountPtr, myX509Certificate, (char *)tableName.c_str());

}

void loop() 
{
  // Actualize Systemtime from ntp if update interval has expired
  DateTime dateTimeUTCNow = sysTime.getTime();
  if (ntp.update())     // if update interval has expired
  {
    dateTimeUTCNow = DateTime((uint16_t) ntp.year(), (uint8_t)ntp.month(), (uint8_t)ntp.day(),
          (uint8_t)ntp.hours(), (uint8_t)ntp.minutes(), (uint8_t)ntp.seconds());

    // Set rtc to UTC Time
    sysTime.setTime(dateTimeUTCNow);
  }
  
  // Create time value to be stored in table rows (local time and offset to UTC) 
  int timeZoneOffsetUTC = ntp.isDST() ? TIMEZONE + DSTOFFSET : TIMEZONE;
  char sampleTime[25] {0};
  createSampleTime(dateTimeUTCNow, timeZoneOffsetUTC, (char *)sampleTime);

  // Create name of the table (arbitrary name + actual year, like: AnalogTestValues2020)
  String tableName = "AnalogTestValues";  
  if (augmentTableNameWithYear)
  {
    tableName += (dateTimeUTCNow.year() - 30);
  }

  // Define 4 sample values to be stored in a table row
  char * sampleValue_1 = (char *)"17.1";
  char * sampleValue_2 = (char *)"17.2";
  char * sampleValue_3 = (char *)"17.3";
  char * sampleValue_4 = (char *)"17.4";

  // Besides PartitionKey and RowKey We have 5 properties to be stored in a row
  // (SampleTime and 4 Samplevalues)
  size_t propertyCount = 5;
  EntityProperty AnalogPropertiesArray[5];
  AnalogPropertiesArray[0] = (EntityProperty)TableEntityProperty((char *)"SampleTime", (char *) sampleTime, (char *)"Edm.String");
  AnalogPropertiesArray[1] = (EntityProperty)TableEntityProperty((char *)"T_1", sampleValue_1, (char *)"Edm.String");
  AnalogPropertiesArray[2] = (EntityProperty)TableEntityProperty((char *)"T_2", sampleValue_2, (char *)"Edm.String");
  AnalogPropertiesArray[3] = (EntityProperty)TableEntityProperty((char *)"T_3", sampleValue_3, (char *)"Edm.String");
  AnalogPropertiesArray[4] = (EntityProperty)TableEntityProperty((char *)"T_4", sampleValue_4, (char *)"Edm.String");
  
  // Create the PartitionKey (special format)
  char partKeySpan[25] {0};
  size_t partitionKeyLength = 0;
  az_span partitionKey = AZ_SPAN_FROM_BUFFER(partKeySpan);
  makePartitionKey(analogTablePartPrefix, augmentPartitionKey, partitionKey, &partitionKeyLength);
  partitionKey = az_span_slice(partitionKey, 0, partitionKeyLength);

  // Create the RowKey (special format)
  char rowKeySpan[25] {0};
  size_t rowKeyLength = 0;
  az_span rowKey = AZ_SPAN_FROM_BUFFER(rowKeySpan);
  makeRowKey(dateTimeUTCNow, rowKey, &rowKeyLength);
  rowKey = az_span_slice(rowKey, 0, rowKeyLength);
  
  // Create TableEntity consisting of the above created incrediants
  AnalogTableEntity analogTableEntity(partitionKey, rowKey, az_span_create_from_str((char *)sampleTime),  AnalogPropertiesArray, propertyCount);
  char EtagBuffer[5] {0};
  
  // Store Entity to Azure Cloud
  az_http_status_code insertResult = insertTableEntity(myCloudStorageAccountPtr, myX509Certificate, (char *)tableName.c_str(), analogTableEntity, (char *)EtagBuffer);
  
  // Wait some time before repeat
  for (int i = 0; i < 5; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);   
    delay(30000);
    digitalWrite(LED_BUILTIN, LOW); 
    delay(30000);
    sprintf(strData, "Wait %u", i);   
    lcd_log_line(strData);
    //Serial.print("Wait ");
    //Serial.println(i);
  }
    sprintf(strData, "   Trying to insert %u", counter);   
    lcd_log_line(strData);
    //Serial.print("   Trying to insert ");
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
 
void makeRowKey(DateTime actDate,  az_span outSpan, size_t *outSpanLength)
{
  // formatting the RowKey (= reverseDate) this way to have the tables sorted with last added row upmost
  char rowKeyBuf[20] {0};
  sprintf(rowKeyBuf, "%4i%02i%02i%02i%02i%02i", (10000 - actDate.year() - 30), (12 - actDate.month()), (31 - actDate.day()), (23 - actDate.hour()), (59 - actDate.minute()), (59 - actDate.second()));
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


az_http_status_code insertTableEntity(CloudStorageAccount *pAccountPtr, X509Certificate pCaCert, const char * pTableName, TableEntity pTableEntity, char * outInsertETag)
{
  TableClient table(pAccountPtr, pCaCert,  httpPtr);

  // Insert Entity
  az_http_status_code statusCode = table.InsertTableEntity(pTableName, pTableEntity,  contApplicationIatomIxml, acceptApplicationIatomIxml, returnContent, false);

  char codeString[25] {0};
  if ((statusCode == AZ_HTTP_STATUS_CODE_NO_CONTENT) || (statusCode == AZ_HTTP_STATUS_CODE_CREATED))
  { 
    sprintf(codeString, "%s %i", "Entity inserted: ", az_http_status_code(statusCode));    
    lcd_log_line((char *)codeString);
  }
  else
  {
    sprintf(codeString, "%s %i", "Insertion failed: ", az_http_status_code(statusCode));    
    lcd_log_line((char *)codeString);
    delay(70000);
  }
}

az_http_status_code createTable(CloudStorageAccount *pAccountPtr, X509Certificate pCaCert, const char * pTableName)
{  
  TableClient table(pAccountPtr, pCaCert,  httpPtr);

  // Create Table
  az_http_status_code statusCode = table.CreateTable(pTableName, contApplicationIatomIxml, acceptApplicationIjson, dont_returnContent, false);
  
  char codeString[25] {0};
  if ((statusCode == AZ_HTTP_STATUS_CODE_CONFLICT) || (statusCode == AZ_HTTP_STATUS_CODE_CREATED))
  { 
    sprintf(codeString, "%s %i", "Table available: ", az_http_status_code(statusCode));    
    lcd_log_line((char *)codeString);
  }
  else
  {
    sprintf(codeString, "%s %i", "Table Creation failed: ", az_http_status_code(statusCode));    
    lcd_log_line((char *)codeString);
    delay(5000);
  }
return statusCode;
}



