#include <AzureStorage/CloudStorageAccount.h>
#include <HTTPClient.h>
#include <AzureStorage/TableClient.h>

String VersionHeader = "2015-04-05";

CloudStorageAccount  * _accountPtr;
HTTPClient * _httpPtr;
const char * _caCert;

TableClient::TableClient(CloudStorageAccount *account, const char * caCert, HTTPClient *httpClient)
{
    _accountPtr = account;
    _caCert = caCert;
    _httpPtr = httpClient;
}
TableClient::~TableClient()
{};

void TableClient::send()
{
    String theUrl = "jsonplaceholder.typicode.com";
    String thePath = "/posts?userId=1";
    
    _httpPtr->begin(theUrl, (uint16_t)443, thePath, _caCert);
    int httpCode = _httpPtr->GET();
    if (httpCode > 0) { //Check for the returning code 
      String payload = _httpPtr->getString();
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
      Serial.println("Error on HTTP request");
      volatile int dummy2 = 1;
      dummy2++;
    }
}