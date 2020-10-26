#include <Arduino.h>
#include <AzureStorage/CloudStorageAccount.h>
#include <HTTPClient.h>
#include <AzureStorage/TableClient.h>
#include <DateTime.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <Encryption/RoSchmi_encryption_helpers.h>
#include <AzureStorage/roschmi_az_storage_tables.h>

#include <AzureStorage/TableEntityProperty.h>
#include <AzureStorage/TableEntity.h>

#include <azure/core/az_span.h>

CloudStorageAccount  * _accountPtr;
HTTPClient * _httpPtr;
const char * _caCert;

static SysTime sysTime;

        
        char * _OperationResponseBody;
        char * _OperationResponseMD5; 
        char * _OperationResponseETag; 

        //private Hashtable _OperationResponseSingleQuery = null;
        //private ArrayList _OperationResponseQueryList = null;



// forward declarations
void GetDateHeader(DateTime, char * stamp, char * x_ms_time);
void appendCharArrayToSpan(az_span targetSpan, const size_t maxTargetLength, const size_t startIndex, size_t *outEndIndex, const char * stringToAppend);
az_span  getContentType_az_span(ContType pContentType);
az_span  getResponseType_az_span(ResponseType pResponseType);
az_span  getAcceptType_az_span(AcceptType pAcceptType);
int base64_decode(const char * input, char * output);
int32_t dow(int32_t year, int32_t month, int32_t day);
void GetTableXml(EntityProperty EntityProperties[], size_t propertyCount, size_t maxPropStrLength, az_span outSpan, size_t *outSpanLength);

void TableClient::CreateTableAuthorizationHeader(char * content, char * canonicalResource, const char * ptimeStamp, String pHttpVerb, az_span pContentType, char * pMD5HashHex, char * pAutorizationHeader, char * pHash, int pHashLen, bool useSharedKeyLite)
{
    char contentTypeString[25] {0};

    az_span_to_str(contentTypeString, (az_span_size(pContentType) + 1), pContentType);                                                                              
    
    if (!useSharedKeyLite)
    {               
        // How to produce Md5 hash:
        //https://community.mongoose-os.com/t/md5-library-setup-and-config-examples/856

        // create Md5Hash           
        const size_t Md5HashStrLenght = 16 + 1;
        char md5HashStr[Md5HashStrLenght] {0};
        int create_Md5_result = createMd5Hash(md5HashStr, Md5HashStrLenght, content);
                
        // Convert to hex-string
        stringToHexString(pMD5HashHex, md5HashStr, (const char *)"");
        String theString = pMD5HashHex;   
    }
                        
    char toSign[(strlen(canonicalResource) + 100)];  // if counted correctly, at least 93 needed
    if (useSharedKeyLite)
    {       
        sprintf(toSign, "%s\%s", (char *)ptimeStamp, canonicalResource);                      
    }
    else
    {     
        sprintf(toSign, "%s\n%s\n%s\n%s\n%s", (char *)pHttpVerb.c_str(), pMD5HashHex , (char *)contentTypeString, (char *)ptimeStamp, canonicalResource);                       
    }
            
    // Produce Authentication Header
    // 1) Base64 decode the Azure Storage Key
           
    // How to decode Base 64 String
    //https://www.ncbi.nlm.nih.gov/IEB/ToolBox/CPP_DOC/lxr/source/src/connect/mbedtls/
            
    // Base64-decode (Azure Storage Key)
                
    char base64DecOut[80] {0};
    int decodeResult = base64_decode(_accountPtr->AccountKey.c_str(), base64DecOut);
                
    size_t decodedKeyLen = (decodeResult != -1) ? decodeResult : 0; 
                                   
    // 2) SHA-256 encode the canonical resource (Here string of: Accountname and 'Tables')
    //    with base-64 deoded Azure Storage Account Key
    // HMAC SHA-256 encoding
    // https://techtutorialsx.com/2018/01/25/esp32-arduino-applying-the-hmac-sha-256-mechanism/
            
    // create SHA256Hash           
    const size_t sha256HashBufferLength = 32 + 1;
    char sha256HashStr[sha256HashBufferLength] {0};
    int create_SHA256_result = createSHA256Hash(sha256HashStr, sha256HashBufferLength, toSign, strlen(toSign), base64DecOut, decodedKeyLen); 
           
    // 3) Base-64 encode the SHA-265 encoded canonical resorce

    const size_t resultBase64Size = 32 + 20;
    char hmacResultBase64[resultBase64Size] {0};
    base64_encode(sha256HashStr, 32, hmacResultBase64, resultBase64Size);
            
    //char retBuf[_accountPtr->AccountName.length() + strlen(hmacResultBase64) + 20] {0};
    char retBuf[MAX_ACCOUNTNAME_LENGTH + resultBase64Size + 20] {0};
    
    if (useSharedKeyLite)
    {  
        //sprintf(pAutorizationHeader, "%s %s:%s", (char *)"SharedKeyLite", (char *)_accountPtr->AccountName.c_str(), hmacResultBase64);                       
        sprintf(retBuf, "%s %s:%s", (char *)"SharedKeyLite", (char *)_accountPtr->AccountName.c_str(), hmacResultBase64);               
        char * ptr = &pAutorizationHeader[0];
        for (size_t i = 0; i < strlen(retBuf); i++) {
            ptr[i] = retBuf[i];
        }
        ptr[strlen(retBuf)] = '\0';
        
        //authorizationHeader = pAutorizationHeader;
        //return authorizationHeader;
    }
    else
    {       
        sprintf(retBuf, "%s %s:%s", (char *)"SharedKey", (char *)_accountPtr->AccountName.c_str(), hmacResultBase64);                
        char * ptr = &pAutorizationHeader[0];
        for (size_t i = 0; i < strlen(retBuf); i++) {
            ptr[i] = retBuf[i];
        }
        ptr[strlen(retBuf)] = '\0';      
    }        
 }



        

// Constructor
TableClient::TableClient(CloudStorageAccount *account, const char * caCert, HTTPClient *httpClient)
//TableClient::TableClient(CloudStorageAccount &account, const char * caCert, HTTPClient *httpClient)
{
    _accountPtr = account;
    //_accountPtr->AccountKey = account->AccountKey;
    //_accountPtr->AccountName = account->AccountName;
    //_accountPtr->UriEndPointTable = account->UriEndPointTable;

    _caCert = caCert;
    _httpPtr = httpClient;
}
TableClient::~TableClient()
{};


void TableClient::send()
{

   
    /*
    String theUrl = "jsonplaceholder.typicode.com";
    String thePath = "/posts?userId=1";


    String theUrl = "prax47.table.core.windows.net";
    String thePath = "/";     
    
    _httpPtr->begin(theUrl, (uint16_t)443, thePath, _caCert);
    int httpCode = _httpPtr->GET();
    if (httpCode > 0) { //Check for the returning code 
      String payload = _httpPtr->getString();
      //lcd_log_line("Http-Code:");
      //lcd_log_line(itoa((int)httpCode, buf, 10));
      int length = payload.length();
      int indexCtr = 0;
      int pageWidth = 30;
      Serial.println(httpCode);
      delay(2000);
      while (indexCtr < length)
      {
        indexCtr += pageWidth;
      } 
     // Serial.println(payload);
    }
    else {
     // Serial.println("Error on HTTP request");
      volatile int dummy2 = 1;
      dummy2++;
    }
    */
}

az_http_status_code TableClient::CreateTable(const char * tableName, ContType pContentType, AcceptType pAcceptType, ResponseType pResponseType, bool useSharedKeyLite)
{
   char * validTableName = (char *)tableName;
   if (strlen(tableName) >  MAX_TABLENAME_LENGTH)
   {
      validTableName[MAX_TABLENAME_LENGTH] = '\0';
   }
   

  //OperationResultsClear();
            
  char x_ms_timestamp[35] {0};
  char timestamp[22] {0};

  GetDateHeader(sysTime.getTime(), timestamp, x_ms_timestamp);

  String timestampUTC = timestamp;
  timestampUTC += ".0000000Z";

  az_span contentTypeAzSpan = getContentType_az_span(pContentType);
  az_span responseTypeAzSpan = getResponseType_az_span(pResponseType);
  az_span acceptTypeAzSpan = getAcceptType_az_span(pAcceptType);

  

  String HttpVerb = "POST";
    
    const char * PROGMEM li1 = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>";
    const char * PROGMEM li2 = "<entry xmlns:d=\"http://schemas.microsoft.com/ado/2007/08/dataservices\"  ";
    const char * PROGMEM li3 = "xmlns:m=\"http://schemas.microsoft.com/ado/2007/08/dataservices/metadata\" ";
    const char * PROGMEM li4 = "xmlns=\"http://www.w3.org/2005/Atom\"> <id>http://";
          char * li5 = (char *)_accountPtr->AccountName.c_str();
    const char * PROGMEM li6 = ".table.core.windows.net/Tables('";
          char * li7 = (char *)validTableName;
    const char * PROGMEM li8 = "')</id><title /><updated>";
          char * li9 = (char *)timestampUTC.c_str();
    const char * PROGMEM li10 = "</updated><author><name/></author> ";
    const char * PROGMEM li11 = "<content type=\"application/xml\"><m:properties><d:TableName>";
          char * li12 = (char *)validTableName;
    const char * PROGMEM li13 = "</d:TableName></m:properties></content></entry>";
           
  char addBuffer[600];
  sprintf(addBuffer, "%s%s%s%s%s%s%s%s%s%s%s%s%s", li1, li2, li3, li4,li5, li6, li7, li8, li9, li10,li11, li12, li13);
  volatile size_t theLength = strlen(addBuffer);

  az_span content_to_upload = az_span_create_from_str(addBuffer);

   String urlPath = validTableName;
   String TableEndPoint = _accountPtr->UriEndPointTable;
   String Host = _accountPtr->HostNameTable;

   String Url = TableEndPoint + "/Tables()";


  char accountName_and_Tables[MAX_ACCOUNTNAME_LENGTH + 15];
  sprintf(accountName_and_Tables, "/%s/%s", (char *)_accountPtr->AccountName.c_str(), (char *)"Tables()");
            
  // RoSchmi todo -> eliminate
  char hushBuffer[5];
  int hashBufferLength = 5;

  char md5Buffer[32 +1] {0};

  //char authorizationHeaderBuffer[strlen(_accountPtr->AccountName.c_str()) + 60] {0};
  char authorizationHeaderBuffer[100] {0};

  CreateTableAuthorizationHeader((char *)addBuffer, accountName_and_Tables, (const char *)x_ms_timestamp, HttpVerb, contentTypeAzSpan, md5Buffer, authorizationHeaderBuffer, hushBuffer, hashBufferLength, useSharedKeyLite = false);

  String authorizationHeader = String((char *)authorizationHeaderBuffer);
  

            /*         
            char canonResourceBuffer[_accountPtr->AccountName.length() + urlPath.length() + 5];
            sprintf(canonResourceBuffer, "/%s/%s", (char *)_accountPtr->AccountName.c_str(), tableName);
            String canonicalizedResource = canonResourceBuffer;
           
            
            char canonHeadersBuffer[50] {0};
            sprintf(canonHeadersBuffer, "Date:%s\nx-ms-date:%s\nx-ms-version:%s", (char *)timestamp.c_str(), (char *)timestamp.c_str(), (char *)VersionHeader.c_str());
            String canonicalizedHeaders = canonHeadersBuffer;
            */
          
            
            
            az_storage_tables_client tabClient;        
            az_storage_tables_client_options options = az_storage_tables_client_options_default();
                        
             if (az_storage_tables_client_init(
          &tabClient, az_span_create_from_str((char *)Url.c_str()), AZ_CREDENTIAL_ANONYMOUS, &options)
      != AZ_OK)
  {
      volatile int dummy643 = 1;
    
  }

   volatile az_span theEndpoint =  tabClient._internal.endpoint;

   uint8_t response_buffer[50] = { 0 };
  az_http_response http_response;
  if (az_result_failed(az_http_response_init(&http_response, AZ_SPAN_FROM_BUFFER(response_buffer))))
  {
     volatile int dummy644 = 1;
  }



  
  if (az_http_response_init(&http_response, AZ_SPAN_FROM_BUFFER(response_buffer)) != AZ_OK)
  {
    volatile int dummy645 = 1;
    
  } 
  az_storage_tables_upload_options uploadOptions = az_storage_tables_upload_options_default();
  
  uploadOptions._internal.acceptType = acceptTypeAzSpan;
  uploadOptions._internal.contentType = contentTypeAzSpan;
  uploadOptions._internal.perferType = responseTypeAzSpan;

// uint8_t request_buffer[1024] = { 0 };
// az_http_request_init
// az_http_request http_request;


  //az_result const blob_upload_result
  //    = az_storage_tables_upload(&tabClient, content_to_upload, az_span_create_from_str(md5Buffer), &uploadOptions, &http_request, &http_response);

    setHttpClient(_httpPtr);
    setCaCert(_caCert);

    

  //az_result const blob_upload_result
  //    = az_storage_tables_upload(&tabClient, content_to_upload, az_span_create_from_str(md5Buffer), az_span_create_from_str((char *)(authorizationHeader.c_str())), az_span_create_from_str((char *)(x_ms_timestamp.c_str())),  &uploadOptions, &http_response);
az_result const blob_upload_result
      = az_storage_tables_upload(&tabClient, content_to_upload, az_span_create_from_str(md5Buffer), az_span_create_from_str((char *)authorizationHeader.c_str()), az_span_create_from_str((char *)x_ms_timestamp), &uploadOptions, &http_response);

    //tabClient._internal.options
   //tabClient._internal.credential->_internal.apply_credential_policy

   volatile int dummy547 = 1;
  
        

           /*
            
            // Uri uri = new Uri(TableEndPoint + "/Tables()");
            Uri uri = new Uri(TableEndPoint + "/Tables()");
 
            var tableTypeHeaders = new Hashtable();
            tableTypeHeaders.Add("Accept-Charset", "UTF-8");
            tableTypeHeaders.Add("MaxDataServiceVersion", "3.0;NetFx");
            tableTypeHeaders.Add("Content-Type", contentType);
            tableTypeHeaders.Add("DataServiceVersion", "3.0");
            tableTypeHeaders.Add("Prefer", getResponseTypeString(pResponseType));
            tableTypeHeaders.Add("Content-MD5", ContentMD5);
            */

 return AZ_HTTP_STATUS_CODE_CREATED;

            /*
            if (_fiddlerIsAttached)
            { AzureStorageHelper.AttachFiddler(_fiddlerIsAttached, _fiddlerIP, _fiddlerPort); }

            BasicHttpResponse response = new BasicHttpResponse();
            try
            {
                AzureStorageHelper.SetDebugMode(_debug);
                AzureStorageHelper.SetDebugLevel(_debug_level);
               
                response = AzureStorageHelper.SendWebRequest(caCerts, uri, authorizationHeader, timestamp, VersionHeader, payload, contentLength, HttpVerb, false, acceptType, tableTypeHeaders);

                return response.StatusCode;
            }
            catch (Exception ex)
            {
                _Print_Debug("Exception was cought: " + ex.Message);
                //Debug.WriteLine("Exception was cought: " + ex.Message);
                response.StatusCode = HttpStatusCode.Forbidden;
                return response.StatusCode;
            }
            */
}
        
 //az_http_status_code TableClient::InsertTableEntity(const char * tableName, TableEntity pEntity, uint32_t pPropertyCount, ContType pContentType, AcceptType pAcceptType, ResponseType pResponseType, bool useSharedKeyLite)
 az_http_status_code TableClient::InsertTableEntity(const char * tableName, TableEntity pEntity, ContType pContentType, AcceptType pAcceptType, ResponseType pResponseType, bool useSharedKeyLite)

{
  char * validTableName = (char *)tableName;
   if (strlen(tableName) >  MAX_TABLENAME_LENGTH)
   {
      validTableName[MAX_TABLENAME_LENGTH] = '\0';
   }


  //pEntity.PartitionKey._internal.size
    
    
    //char PartitionKey[pEntity.PartitionKey._internal.size + 2] {0};
    char PartitionKey[15] {0};
    az_span_to_str(PartitionKey, sizeof(PartitionKey) - 1, pEntity.PartitionKey);

    
    //char RowKey[pEntity.RowKey._internal.size + 2] {0};
    char RowKey[16] {0};
    az_span_to_str(RowKey, sizeof(RowKey), pEntity.RowKey);

    
    //char SampleTime[pEntity.SampleTime._internal.size + 2] {0};
    char SampleTime[26] {0};
    az_span_to_str(SampleTime, sizeof(SampleTime), pEntity.SampleTime);
    
    
    //EntityProperty * Properties;
    //Properties = pEntity.Properties;
    
    
    
    //az_span Name = az_span_create_from_str((char *)pEntity.Properties[0].Name);

  char x_ms_timestamp[35] {0};
  char timestamp[22] {0};

  GetDateHeader(sysTime.getTime(), timestamp, x_ms_timestamp);

  String timestampUTC = timestamp;
  timestampUTC += ".0000000Z";

  az_span contentTypeAzSpan = getContentType_az_span(pContentType);
  az_span responseTypeAzSpan = getResponseType_az_span(pResponseType);
  az_span acceptTypeAzSpan = getAcceptType_az_span(pAcceptType);

   
   const size_t maxPropStrLength = 70; // max size of a property string
   size_t outBytesWritten = 0;
   

//   char  propBuf[300];
//   az_span outPropertySpan = AZ_SPAN_FROM_BUFFER(propBuf);

// To save memory allocate to address 0x20029000
uint8_t * BufAddress = (uint8_t *)0x20029000;
az_span outPropertySpan = az_span_create(BufAddress, 300);

   GetTableXml(pEntity.Properties, pEntity.PropertyCount, maxPropStrLength, outPropertySpan, &outBytesWritten);
   

   char * _properties = (char *)az_span_ptr(outPropertySpan);

   /*
   uint8_t newBuffer[100];
   uint8_t * BufAddress = (uint8_t *)newBuffer;
   BufAddress = (uint8_t *)0x20029000;
   *BufAddress = 0x51;
   az_span newSpan = az_span_create((uint8_t *)newBuffer, 100);
   */

   //char * _properties = (char *)"<Hello></Hello>";
  
   //String props = _properties;
   

   String HttpVerb = "POST";
   
  //**************************************************************
  /*

    const char * li1 = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>";
    const char * li2 = "<entry xmlns:d=\"http://schemas.microsoft.com/ado/2007/08/dataservices\"  ";
    const char * li3 = "xmlns:m=\"http://schemas.microsoft.com/ado/2007/08/dataservices/metadata\" ";
    const char * li4 = "xmlns=\"http://www.w3.org/2005/Atom\"> <id>http://";
          char * li5 = (char *)_accountPtr->AccountName.c_str();
    const char * li6 = ".table.core.windows.net/Tables('";
          char * li7 = (char *)validTableName;
    const char * li8 = "')</id><title /><updated>";
    char * li9 = (char *)timestampUTC.c_str();
    const char * li10 = "</updated><author><name/></author> ";
    const char * li11 = "<content type=\"application/xml\"><m:properties><d:TableName>";
          char * li12 = (char *)validTableName;
    const char * li13 = "</d:TableName></m:properties></content></entry>";
           
  char addBuffer[600];
  sprintf(addBuffer, "%s%s%s%s%s%s%s%s%s%s%s%s%s", li1, li2, li3, li4,li5, li6, li7, li8, li9, li10,li11, li12, li13);
  volatile size_t theLength = strlen(addBuffer);

  az_span content_to_upload = az_span_create_from_str(addBuffer);

   String urlPath = tableName;
   String TableEndPoint = _accountPtr->UriEndPointTable;
   String Host = _accountPtr->HostNameTable;

   String Url = TableEndPoint + "/Tables()";

*/
//**************************************************************






//*******************************************************


  
  const char * PROGMEM li1 = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>";
  const char * PROGMEM li2 = "<entry xmlns:d=\"http://schemas.microsoft.com/ado/2007/08/dataservices\"  ";
  const char * PROGMEM li3 = "xmlns:m=\"http://schemas.microsoft.com/ado/2007/08/dataservices/metadata\" ";    
  const char * PROGMEM li4  = "xmlns=\"http://www.w3.org/2005/Atom\"> <id>http://";
        char * li5  = (char *)_accountPtr->AccountName.c_str();
  const char * PROGMEM li6  = ".table.core.windows.net/";
        char *  li7  = (char *)validTableName;
  const char * PROGMEM li8  = "(PartitionKey='";
        char * li9  = (char *)PartitionKey; 
  const char * PROGMEM li10  = "',RowKey='";
        char * li11  = (char *)RowKey; 
  const char * PROGMEM li12  = "')</id><title /><updated>";
        char * li13  = (char *)x_ms_timestamp;
  const char * PROGMEM li14  = "</updated><author><name /></author><content type=\"application/atom+xml\">";
  const char * PROGMEM li15  = "<m:properties><d:PartitionKey>";
        char * li16  = (char *)PartitionKey;
  const char * PROGMEM li17  =  "</d:PartitionKey><d:RowKey>";
        char * li18  = (char *)RowKey;
  const char * PROGMEM li19  = "</d:RowKey>";  
        char * li20  = _properties;
  const char * PROGMEM li21  = "</m:properties></content></entry>";

  //char addBuffer[900];
   uint8_t addBuffer[1];
   uint8_t * addBufAddress = (uint8_t *)addBuffer;
   addBufAddress = (uint8_t *)0x20029200;

   az_span startContent_to_upload = az_span_create(addBufAddress, 900);

   size_t outIndex = 0;

   uint8_t remainderBuffer[1];
   uint8_t * remainderBufAddress = (uint8_t *)remainderBuffer;
   remainderBufAddress = (uint8_t *)0x20029700;
   az_span remainder = az_span_create(remainderBufAddress, 900);

   
            remainder = az_span_copy(startContent_to_upload, az_span_create_from_str((char *)li1));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li2));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li3));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li4));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li5));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li6));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li7));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li8));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li9));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li10));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li11));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li12));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li13));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li14));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li15));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li16));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li17));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li18));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li19));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li20));
            remainder = az_span_copy(remainder, az_span_create_from_str((char *)li21));
    
az_span_copy_u8(remainder, 0);

   /*
   
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li1);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li2);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li3);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li4);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li5);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li6);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li7);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li8);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li9);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li10);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li11);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li12);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li13);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li14);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li15);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li16);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li17);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li18);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li19);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li20);
   appendCharArrayToSpan(content_to_upload, 900, 0, &outIndex, li21);
   */

   //char * contToUpl = (char *)addBufAddress;

   //String toUpload = contToUpl;

  //char addBuffer[650];
  //char addBuffer[700];

  //sprintf(addBuffer, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", li1, li2, li3, li4,li5, li6, li7, li8, li9, li10,li11, li12, li13, li14, li15, li16, li17, li18, li19, li20, li21); 
  //sprintf(addBuffer, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", li1, li2, li3, li4,li5, li6, li7, li8, li9, li10,li11, li12, li13, li14, li15, li16, li17, li18, li19, li20, li21, (char *)"    ");
  //volatile size_t buffLength = strlen(addBuffer);

  az_span content_to_upload = az_span_create_from_str((char *)addBufAddress);   
             
       

   String urlPath = validTableName;
   String TableEndPoint = _accountPtr->UriEndPointTable;
   String Host = _accountPtr->HostNameTable;

   String Url = TableEndPoint + "/" + urlPath + "()";
   
   


   //**********************************************************

   //RoSchmi: For tests
   //String Url = TableEndPoint + "/Tables()";


    //resource = "/Tables()";


char accountName_and_Tables[MAX_ACCOUNTNAME_LENGTH + MAX_TABLENAME_LENGTH + 10];
 sprintf(accountName_and_Tables, "/%s/%s%s", (char *)_accountPtr->AccountName.c_str(), validTableName, (const char *)"()");
 //sprintf(accountName_and_Tables, "/%s/%s", (char *)_accountPtr->AccountName.c_str(), validTableName);
 // RoSchmi todo -> eliminate
  char hushBuffer[5];
  int hashBufferLength = 5;

  char md5Buffer[32 +1] {0};

  //char authorizationHeaderBuffer[strlen(_accountPtr->AccountName.c_str()) + 60] {0};
  char authorizationHeaderBuffer[65] {0};

  CreateTableAuthorizationHeader((char *)addBufAddress, accountName_and_Tables, (const char *)x_ms_timestamp, HttpVerb, contentTypeAzSpan, md5Buffer, authorizationHeaderBuffer, hushBuffer, hashBufferLength, useSharedKeyLite = false);

  //String authorizationHeader = String((char *)authorizationHeaderBuffer);
  
  volatile int initResult = 55;         
            az_storage_tables_client tabClient;        
            az_storage_tables_client_options options = az_storage_tables_client_options_default();
                        
             if (az_storage_tables_client_init(
          &tabClient, az_span_create_from_str((char *)Url.c_str()), AZ_CREDENTIAL_ANONYMOUS, &options)
      != AZ_OK)
  {
      volatile int dummy643 = 1;
      initResult = -1;
  }
  else
  {
    initResult = 0;
  }
  

volatile az_span theEndpoint =  tabClient._internal.endpoint;

   uint8_t response_buffer[2] = { 0 };
  az_http_response http_response;
  if (az_result_failed(az_http_response_init(&http_response, AZ_SPAN_FROM_BUFFER(response_buffer))))
  {
     volatile int dummy644 = 1;
  }
   /*
   volatile uint32_t * ptr_one;
   volatile uint32_t * last_ptr_one;
   for (volatile int i = 0; 1 < 100000; i++)
   {
     last_ptr_one = ptr_one;
     ptr_one = 0;
     ptr_one = (uint32_t *)malloc(1);
     if (ptr_one == 0)
     {
       ptr_one = last_ptr_one;
       volatile int dummy68424 = 1;
     }
     else
     {
       *ptr_one = (uint32_t)0xAA55AA55;
     } 
   }
  */
  
  if (az_http_response_init(&http_response, AZ_SPAN_FROM_BUFFER(response_buffer)) != AZ_OK)
  {
    volatile int dummy645 = 1;
    
  } 
  az_storage_tables_upload_options uploadOptions = az_storage_tables_upload_options_default();
  
  uploadOptions._internal.acceptType = acceptTypeAzSpan;
  uploadOptions._internal.contentType = contentTypeAzSpan;
  uploadOptions._internal.perferType = responseTypeAzSpan;

 setHttpClient(_httpPtr);
    setCaCert(_caCert);

    

  //az_result const blob_upload_result
  //    = az_storage_tables_upload(&tabClient, content_to_upload, az_span_create_from_str(md5Buffer), az_span_create_from_str((char *)(authorizationHeader.c_str())), az_span_create_from_str((char *)(x_ms_timestamp.c_str())),  &uploadOptions, &http_response);
az_result const blob_upload_result
      = az_storage_tables_upload(&tabClient, content_to_upload, az_span_create_from_str(md5Buffer), az_span_create_from_str((char *)authorizationHeaderBuffer), az_span_create_from_str((char *)x_ms_timestamp), &uploadOptions, &http_response);

    //tabClient._internal.options
   //tabClient._internal.credential->_internal.apply_credential_policy

   volatile int dummy547 = 1;
    volatile int dumy4 = 1;

  return AZ_HTTP_STATUS_CODE_CREATED;
}      

void appendCharArrayToSpan(az_span targetSpan, const size_t maxTargetLength, const size_t startIndex, size_t *outEndIndex, const char * stringToAppend)
{
    //uint8_t* ptr = az_span_ptr(targetSpan);
    //uint32_t ptr = (uint32_t)targetSpan._internal.ptr;
    
    //uint8_t destBuffer[targetSpan._internal.size];
    //az_span destination = AZ_SPAN_FROM_BUFFER(destBuffer);

    uint8_t * lastPtr = targetSpan._internal.ptr;
    int32_t lastSize = targetSpan._internal.size;
    az_span_copy(targetSpan, az_span_create_from_str((char *)stringToAppend));
    targetSpan = (az_span){ ._internal = { .ptr = lastPtr + strlen(stringToAppend), .size = lastSize - (int32_t)strlen(stringToAppend) } };
    
    //targetSpan._internal.ptr += strlen(stringToAppend);
    //targetSpan._internal.size -= strlen(stringToAppend);
    //return (az_span){ ._internal = { .ptr = ptr, .size = size } };
    volatile int dummy34 = 1;
    //targetSpan = destination;
    //targetSpan._internal.ptr += strlen(stringToAppend);
    //az_span_copy_u8(targetSpan, 0);
    //*outEndIndex = startIndex + 
}

       void OperationResultsClear()
        {
            //_OperationResponseETag = null;
            //_OperationResponseBody = null;
            //_OperationResponseMD5 = null;
            //_OperationResponseSingleQuery = null;
            //_OperationResponseQueryList = null;
        }

        
        void GetTableXml(EntityProperty EntityProperties[], size_t propertyCount, size_t maxPropStrLength, az_span outSpan, size_t *outSpanLength)
        { 
            char prop[(MAX_ENTITYPROPERTY_NAME_LENGTH * 2) + MAX_ENTITYPROPERTY_VALUE_LENGTH + MAX_ENTITYPROPERTY_TYPE_LENGTH + 20] {0};          
            

            
            
            
            size_t outLength = 0;
            for (size_t i = 0; i < propertyCount; i++)
            {
              // RoSchmi ToDo: define precondition      
              //strcpy(prop, EntityProperties[i].Prefix);
              sprintf(prop, "<d:%s m:type=%c%s%c>%s</d:%s>", EntityProperties[i].Name, '"', EntityProperties[i].Type, '"', EntityProperties[i].Value, EntityProperties[i].Name);
              if ((prop != NULL) && (strlen(prop) > 0))
              {                 
                  outSpan = az_span_copy(outSpan, az_span_create_from_str((char *)prop));
                  outLength += strlen((char *)prop);    
              }           
            }
            az_span_copy_u8(outSpan, 0);
            *outSpanLength = outLength;

            //remainder = az_span_copy(startContent_to_upload, az_span_create_from_str((char *)li1));
            //remainder = az_span_copy(remainder, az_span_create_from_str((char *)li2));

//uint8_t * remainBuf = (uint8_t *)0x20029000;

            /*
            uint8_t * remainBuf[300];

            az_span remainder = AZ_SPAN_FROM_BUFFER(remainBuf);
              size_t outLength = 0;
            for (size_t i = 0; i < propertyCount; i++)
            {
              remainder = az_span_copy(outSpan, az_span_create_from_str((char *)"<d:"));           
              remainder = az_span_copy(remainder, az_span_create_from_str(EntityProperties[i].Name));
              remainder = az_span_copy(remainder, az_span_create_from_str((char *)" m:type="));
              remainder = az_span_copy(remainder, az_span_create_from_str((char *)'"'));
              remainder = az_span_copy(remainder, az_span_create_from_str(EntityProperties[i].Type));
              remainder = az_span_copy(remainder, az_span_create_from_str((char *)'"'));
              remainder = az_span_copy(remainder, az_span_create_from_str((char *)">"));
           
            }
            */
        }
        
        void GetDateHeader(DateTime time, char * stamp, char * x_ms_time)
        {
           

int32_t dayOfWeek = dow((int32_t)time.year() -30, (int32_t)time.month(), (int32_t)time.day());

          struct tm timeinfo {
                        (int)time.second(),
                        (int)time.minute(),
                        (int)time.hour(),
                        (int)time.day(),
                        (int)time.month(),
                        (int)(time.year() - 1900 - 30),                       
                        (int)dayOfWeek,
                        0,                               
                        0};

        int monthSave = (int)time.month();

          // RoSchmi: There seems to be a bug in the strftime function selecting the month name
          // January is taken for 0, it should be taken for 1
          // Here I use a bugfix to correct the association 
          
        
        // Check if 1 is associated with Feb (instead of Jan)
        // If true -> decrement by 1
       
        timeinfo.tm_mon = 1;
        strftime((char *)x_ms_time, 35, "%b", &timeinfo);
        
        timeinfo.tm_mon =  (strcmp(x_ms_time, (char *)"Feb") == 0) ? (monthSave - 1) : monthSave;
                  
          strftime((char *)x_ms_time, 35, "%a, %d %b %Y %H:%M:%S GMT", &timeinfo);
          strftime((char *)stamp, 22, "%Y-%m-%dT%H:%M:%S", &timeinfo);
       
        }

        az_span getContentType_az_span(ContType pContentType)
        {           
            if (pContentType == contApplicationIatomIxml)
            { 
              return AZ_SPAN_LITERAL_FROM_STR("application/atom+xml");          
            }
            else
            { 
              return AZ_SPAN_LITERAL_FROM_STR("application/json"); 
            }
        }

        az_span getAcceptType_az_span(AcceptType pAcceptType)
        {
          if (pAcceptType == acceptApplicationIatomIxml)
            { return AZ_SPAN_LITERAL_FROM_STR("application/atom+xml"); }
            else
            { return AZ_SPAN_LITERAL_FROM_STR("application/json"); }
        }
        
        az_span getResponseType_az_span(ResponseType pResponseType)
        {
          if (pResponseType == returnContent)
            { return AZ_SPAN_LITERAL_FROM_STR("return-content"); }
            else
            { return AZ_SPAN_LITERAL_FROM_STR("return-no-content"); }
        }

        /* Returns the number of days to the start of the specified year, taking leap
 * years into account, but not the shift from the Julian calendar to the
 * Gregorian calendar. Instead, it is as though the Gregorian calendar is
 * extrapolated back in time to a hypothetical "year zero".
 */
int leap (int year)
{
  return year*365 + (year/4) - (year/100) + (year/400);
}

/* Returns a number representing the number of days since March 1 in the
 * hypothetical year 0, not counting the change from the Julian calendar
 * to the Gregorian calendar that occured in the 16th century. This
 * algorithm is loosely based on a function known as "Zeller's Congruence".
 * This number MOD 7 gives the day of week, where 0 = Monday and 6 = Sunday.
 */
int32_t zeller (int32_t year, int32_t month, int32_t day)
{
  year += ((month+9)/12) - 1;
  month = (month+9) % 12;
  return leap (year) + month*30 + ((6*month+5)/10) + day + 1;
}

/* Returns the day of week (1=Monday, 7=Sunday) for a given date.
 */
int32_t dow (int32_t year, int32_t month, int32_t day)
{
  return (zeller (year, month, day) % 7) + 1;
}
