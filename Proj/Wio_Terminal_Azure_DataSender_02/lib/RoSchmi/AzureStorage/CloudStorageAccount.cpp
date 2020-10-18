// This Class is only for tests, can be deleted

#include <Arduino.h>
#include <AzureStorage/CloudStorageAccount.h>

/**
 * constructor
 */
CloudStorageAccount::CloudStorageAccount(String accountName, String accountKey, bool useHttps )
{
    AccountName = accountName;
    AccountKey = accountKey;
    char strData[accountName.length() + 30];
    const char * insert = (char *)useHttps ? "s" : "";

    sprintf(strData, "http%s://%s.table.core.windows.net", insert, accountName.c_str());
    UriEndPointTable = String(strData);
    //sprintf(strData, "http%s://%s.blob.core.windows.net", insert, accountName.c_str());
    //UriEndPointBlob = String(strData);
    //sprintf(strData, "http%s://%s.queue.core.windows.net", insert, accountName.c_str());
    //UriEndPointQueue = String(strData);

    sprintf(strData, "%s.table.core.windows.net", accountName.c_str());
    HostNameTable = String(strData);
    //sprintf(strData, "%s.blob.core.windows.net", accountName.c_str());
    //HostNameBlob = String(strData);
    //sprintf(strData, "%s.queue.core.windows.net", accountName.c_str());
    //HostNameQueue = String(strData);
}
/**
 * destructor
 */
CloudStorageAccount::~CloudStorageAccount()
{}