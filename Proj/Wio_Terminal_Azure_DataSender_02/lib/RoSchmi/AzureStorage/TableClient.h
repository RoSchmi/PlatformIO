#include <AzureStorage/CloudStorageAccount.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <azure/core/az_http.h>
#include <AzureStorage/SysTime.h>

#include "DateTime.h"

#ifndef _TABLECLIENT_H_
#define _TABLECLIENT_H_

  typedef enum {
    contApplicationIatomIxml,
    contApplicationIjson
    } ContType;


typedef enum 
        {
            acceptApplicationIatomIxml,
            acceptApplicationIjson
        } AcceptType;


typedef enum 
        {
            returnContent,
            dont_returnContent
        } ResponseType;



class TableClient
{
public:
    TableClient(CloudStorageAccount *account, const char * caCert, HTTPClient *httpClient);
    ~TableClient();
    
    void send();

    az_http_status_code CreateTable(const char * tableName, ContType pContentType = ContType::contApplicationIatomIxml, AcceptType pAcceptType = AcceptType::acceptApplicationIjson, ResponseType pResponseType = ResponseType::returnContent, bool useSharedKeyLight = false);
};
#endif 