// This Class is only for tests, can be deleted

#include <AzureStorage/CloudStorageAccount.h>
#include <AzureStorage/TestPassInteger.h>


int intValue = 0;

TestPassInteger::TestPassInteger(int pIntergerValue)
{
    intValue = pIntergerValue;   
    
}

int getValue()
{
    return intValue;
}