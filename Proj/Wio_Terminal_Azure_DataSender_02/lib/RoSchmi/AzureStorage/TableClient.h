#include <AzureStorage/CloudStorageAccount.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#ifndef _TABLECLIENT_H_
#define _TABLECLIENT_H_

class TableClient
{
public:
    TableClient(CloudStorageAccount *account, const char * caCert, HTTPClient *httpClient);
    ~TableClient();
    
    void send();  
};
#endif 