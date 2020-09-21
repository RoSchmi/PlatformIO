#include <AzureStorage/CloudStorageAccount.h>
#include <AzureStorage/TestPassAccount.h>



//CloudStorageAccount _account;

TestPassAccount::TestPassAccount(CloudStorageAccount account)
{
    _account = account;   
    
}

String getUrl()
{
    return TestPassAccount::_account.UriEndPointTable;
}
