#include <Arduino.h>
#include <AzureStorage/CloudStorageAccount.h>

#ifndef _TESTPASSACCOUNT_H_
#define _TESTPASSACCOUNT_H_

class TestPassAccount
{
public:
    TestPassAccount(CloudStorageAccount account);
    ~TestPassAccount();

    String getUrl();

    static CloudStorageAccount _account;
};

#endif 