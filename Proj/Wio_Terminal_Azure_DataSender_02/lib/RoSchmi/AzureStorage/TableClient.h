#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "DateTime.h"

#include "mbedtls/md.h"
#include "mbedtls/md5.h"
#include "mbedtls/base64.h"
#include "mbedtls/sha256.h"

#include <azure/core/az_http.h>

#include <Time/SysTime.h>
#include <AzureStorage/CloudStorageAccount.h>
#include <AzureStorage/TableEntity.h>

#include <az_wioterminal_roschmi.h>

#ifndef _TABLECLIENT_H_
#define _TABLECLIENT_H_

#define MAX_TABLENAME_LENGTH 50
#define RESPONSE_BUFFER_MEMORY_ADDR 0x2002A000
#define RESPONSE_BUFFER_LENGTH 2000
#define REQUEST_BODY_BUFFER_MEMORY_ADDR 0x20029200
#define REQUEST_BODY_BUFFER_LENGTH 900
#define PROPERTIES_BUFFER_MEMORY_ADDR 0x20029000
#define PROPERTIES_BUFFER_LENGTH 300

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
    //TableClient(CloudStorageAccount *account, const char * caCert, HTTPClient *httpClient, WiFiClientSecure *wifiClient);
    ~TableClient();
    
    void send();

    az_http_status_code CreateTable(const char * tableName, ContType pContentType = ContType::contApplicationIatomIxml, AcceptType pAcceptType = AcceptType::acceptApplicationIjson, ResponseType pResponseType = ResponseType::returnContent, bool useSharedKeyLight = false);
    az_http_status_code InsertTableEntity(const char * tableName, TableEntity pEntity, ContType pContentType, AcceptType pAcceptType, ResponseType pResponseType, bool useSharedKeyLite = false);   
    void CreateTableAuthorizationHeader(const char * content, const char * canonicalResource, const char * ptimeStamp, const char * pHttpVerb, az_span pConentType, char * pMd5Hash, char pAutorizationHeader[], bool useSharedKeyLite = false);
    int32_t dow(int32_t year, int32_t month, int32_t day);
};
#endif 