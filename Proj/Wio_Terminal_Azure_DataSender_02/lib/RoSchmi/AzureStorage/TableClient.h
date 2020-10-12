#include <Arduino.h>
#include <AzureStorage/CloudStorageAccount.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <azure/core/az_http.h>
#include <Time/SysTime.h>


#include "mbedtls/md.h"
#include "mbedtls/md5.h"
#include "mbedtls/base64.h"
#include "mbedtls/sha256.h"

#include "DateTime.h"

#include <az_wioterminal_roschmi.h>

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
    //void CreateTableAuthorizationHeader(char * content, char * canonicalResource, const char * ptimeStamp, String pHttpVerb, ContType pContentType, char * pMd5Hash, char pAutorizationHeader[], char * pHash, int pHashLength, bool useSharedKeyLite = false);
    void CreateTableAuthorizationHeader(char * content, char * canonicalResource, const char * ptimeStamp, String pHttpVerb, az_span pConentType, char * pMd5Hash, char pAutorizationHeader[], char * pHash, int pHashLength, bool useSharedKeyLite = false);
    int32_t dow(int32_t year, int32_t month, int32_t day);
};
#endif 