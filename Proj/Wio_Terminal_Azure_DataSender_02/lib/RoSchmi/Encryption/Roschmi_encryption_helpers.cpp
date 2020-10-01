#include <Arduino.h>
#include <Encryption/RoSchmi_encryption_helpers.h>
//#include <AzureStorage/az_iot_helpers.h>

//#include <azure/core/az_span.h>
//#include <azure/core/az_result.h>

//#include "NTP.h"
//#include <time.h>



#include "mbedtls/md.h"
#include "mbedtls/base64.h"
#include "mbedtls/sha256.h"


int createMd5Hash(char * output17Bytes, size_t outputLength, const char * input)
{
    uint8_t md5hash[16];  
           
    if (outputLength >= 17)
    {
        mbedtls_md5_context ctxMd5;
        mbedtls_md5_init(&ctxMd5);
        mbedtls_md5_starts(&ctxMd5);
        mbedtls_md5_update(&ctxMd5, (const uint8_t*) input, strlen(input));           
        mbedtls_md5_finish(&ctxMd5, md5hash);
                /*
                char md5HashStr[17] {0};
                char * ptr = &md5HashStr[0];
                for (int i = 0; i < 16; i++) {
                    ptr[i] = md5hash[i];
                }
                */

        char * ptr = &output17Bytes[0];
        for (int i = 0; i < 16; i++) {
            ptr[i] = md5hash[i];
        }
        ptr[16] = '\0';

        mbedtls_md5_free(&ctxMd5);
        return 0;
    }
    else
    {
        return 1;
    }  



}

void stringToHexString(char * output, const char * input, const char * delimiter = "")
        {
            const char _hexCharacterTable[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };  
            size_t charsCount = (strlen(input) * 2) + 1 + strlen(input - 1) * strlen(delimiter);          
            char chars[charsCount];
            int j = 0;
                for (size_t i = 0; i < strlen(input); i++)
                {
                    chars[j++] = (char)_hexCharacterTable[(input[i] & 0xF0) >> 4];
                    chars[j++] = (char)_hexCharacterTable[input[i] & 0x0F];
                    
                    if (i != (strlen(input) - 1))
                    {                       
                            for (size_t i = 0; i < strlen(delimiter); i++)
                           {                            
                            chars[j++] = delimiter[i];                            
                           }                     
                    }                  
                }
                chars[j] = '\0';

               output[charsCount] = '\0';
                for (int i = 0; i < charsCount; i++) {
                    output[i] = chars[i];
                }
        }

// returns -1 if something went wrong, otherwise count of bytes written
int base64_decode(const char * input, char * output)
{

    size_t destLen = strlen(input);  // size of the destination buffer
    char base64DecOut[destLen];     // destination buffer (can be NULL for checking size)
            
            size_t olen = 0;         // number of bytes written 

            mbedtls_base64_decode((unsigned char *)base64DecOut, destLen, &olen, (unsigned char *)input, strlen(input));
            
            if (olen == 0)
            {
                return -1;
            }
            else
            {
                // RoSchmi Todo corect calculation of ctr
                //size_t ctr = olen > sizeof(output) ? (sizeof(output) -1) : olen;
                size_t ctr = olen;
                output[ctr] = '\0';
                for (int i = 0; i < ctr; i++) {
                    output[i] = base64DecOut[i];
                }                
                return olen;
            }
}

int base64_encode(const char * input, char * output)
{

    size_t outPutBufferLength = strlen(input) * 3;  // size of the destination buffer
    char base64EncOut[outPutBufferLength];     // destination buffer (can be NULL for checking size)
            
            size_t outPutWritten = 0;         // number of bytes written 


           // mbedtls_base64_decode((unsigned char *)base64DecOut, destLen, &olen, (unsigned char *)input, strlen(input));
            
            int Base64EncodeResult = mbedtls_base64_encode((unsigned char *)base64EncOut, outPutBufferLength, &outPutWritten,
                   (const unsigned char *)input, strlen(input));

            if (outPutWritten == 0)
            {
                return 1;
            }
            else
            {
                // RoSchmi Todo correct calculation of ctr
                //size_t ctr = outPutWritten > sizeof(output) ? (sizeof(output) -1) : outPutWritten;
                size_t ctr = outPutWritten;
                output[ctr] = '\0';
                for (int i = 0; i < ctr; i++) {
                    output[i] = base64EncOut[i];
                }                
                return 0;
            }
}