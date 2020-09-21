#include <AzureStorage/CloudStorageAccount.h>
#include <HTTPClient.h>
//#include <config.h>
#include <AzureStorage/TableClient.h>



CloudStorageAccount  * _accountPtr;
String VersionHeader = "2015-04-05";
HTTPClient * httpPtrLocal;
WiFiClientSecure * wifi_clientPtr;

X509CertificateArray * _X509CertificateArrayPtr;
X509Certificate  _X509CertificatePtr;

const char * _caCert;


//TableClient::TableClient(CloudStorageAccount &account, X509CertificateArray &certArray, HTTPClient &httpClient, WiFiClientSecure &wifiClient)
//TableClient::TableClient(CloudStorageAccount *account, X509CertificateArray *certArray, HTTPClient *httpClient, WiFiClientSecure *wifiClient)
//TableClient::TableClient(CloudStorageAccount *account, X509CertificateArray *certArray, HTTPClient *httpClient)
//TableClient::TableClient(CloudStorageAccount *account, const char * caCert, HTTPClient *httpClient)
TableClient::TableClient(CloudStorageAccount *account, const char * caCert, HTTPClient *httpClient)
{
    /*
    _account = &account;
    _X509CertificateArray = &certArray;  
    http = &httpClient;
    wifi_client = &wifiClient;
    */
    _accountPtr = account;
    
    _X509CertificatePtr = caCert;

    _caCert = caCert;
    httpPtrLocal = httpClient;
   // wifi_clientPtr = wifiClient;

}
TableClient::~TableClient()
{};

void TableClient::send()
{
    String theUrl = "jsonplaceholder.typicode.com";
    String thePath = "/posts?userId=1";
    
    httpPtrLocal->begin(theUrl, (uint16_t)443, thePath, _caCert);
    //httpPtrLocal->begin(theUrl, (uint16_t)443, thePath, baltimore_root_ca);

    int httpCode = httpPtrLocal->GET();
    if (httpCode > 0) { //Check for the returning code 
      String payload = httpPtrLocal->getString();
      //lcd_log_line("Http-Code:");
      //lcd_log_line(itoa((int)httpCode, buf, 10));
      int length = payload.length();
      int indexCtr = 0;
      int pageWidth = 30;
      Serial.println(httpCode);
      delay(2000);
      while (indexCtr < length)
      {
        //lcd_log_line((char*)payload.substring(indexCtr, indexCtr + pageWidth).c_str());
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