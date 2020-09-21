// This Class is only for tests, can be deleted

#include <Arduino.h>
#include <AzureStorage/CloudStorageAccount.h>


/**
 * constructor
 */
CloudStorageAccount::CloudStorageAccount(String accountName, String accountKey, bool useHttps )
{
    AccountName = accountName;
    accountKey = accountKey;
    char strData[accountName.length() + 50];
    const char * insert = (char *)useHttps ? "s" : "";

    sprintf(strData, "http%s://%s.table.core.windows.net", insert, accountName.c_str());
    UriEndPointTable = String(strData);
    sprintf(strData, "http%s://%s.blob.core.windows.net", insert, accountName.c_str());
    UriEndPointBlob = String(strData);
    sprintf(strData, "http%s://%s.queue.core.windows.net", insert, accountName.c_str());
    UriEndPointQueue = String(strData);
}

/**
 * destructor
 */
CloudStorageAccount::~CloudStorageAccount()
{}