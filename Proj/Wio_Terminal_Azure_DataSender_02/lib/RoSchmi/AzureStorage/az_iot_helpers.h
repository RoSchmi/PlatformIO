#ifndef AZ_IOT_HELPERS_H
#define AZ_IOT_HELPERS_H

#include <azure/iot/az_iot_hub_client.h>
#include <azure/core/az_result.h>
#include <azure/core/az_span.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

int generateSasToken(const az_iot_hub_client *az_iot_hub_client, const char *device_key, uint32_t expiration, char *sas_token, size_t size);
//int connectToAzureIoTHub(const az_iot_hub_client *az_iot_hub_client);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif