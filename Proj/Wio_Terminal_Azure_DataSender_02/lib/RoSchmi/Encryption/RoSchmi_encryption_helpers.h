
#include "mbedtls/md.h"
#include "mbedtls/md5.h"
#include "mbedtls/base64.h"
#include "mbedtls/sha256.h"

#ifndef _ROSCHMI_ENCRYPTION_HELPERS_H_
#define _ROSCHMI_ENCRYPTION_HELPERS_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

int createMd5Hash(char * output16Bytes, size_t md5HashStrLenght, const char * input);

void stringToHexString(char * output, const char * input, const char * delimiter); 

int base64_decode(const char * input, char * output);

int base64_encode(const char * input, char * output);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif




