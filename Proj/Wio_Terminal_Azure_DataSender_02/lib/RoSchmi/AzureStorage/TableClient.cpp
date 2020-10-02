#include <Arduino.h>
#include <AzureStorage/CloudStorageAccount.h>
#include <HTTPClient.h>
#include <AzureStorage/TableClient.h>
#include <DateTime.h>
#include <stdio.h>
#include <stdlib.h>
#include <Encryption/RoSchmi_encryption_helpers.h>


String VersionHeader = "2015-04-05";

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
String GetDateHeader();
String getContentTypeString(ContType pContentType);
String getAcceptTypeString(AcceptType pAcceptType);
int base64_decode(const char * input, char * output);

String TableClient::CreateTableAuthorizationHeader(char * content, char * canonicalResource, String ptimeStamp, String pHttpVerb, ContType pContentType, char * pMD5HashHex, char * pHash, int pHashLen, bool useSharedKeyLite)
{
    String contentType = getContentTypeString(pContentType);
                                                                                  
    if (!useSharedKeyLite)
    {               
        // How to produce Md5 hash:
        //https://community.mongoose-os.com/t/md5-library-setup-and-config-examples/856

        // create Md5Hash           
        size_t Md5HashStrLenght = 16 + 1;
        char md5HashStr[Md5HashStrLenght] {0};
        int create_Md5_result = createMd5Hash(md5HashStr, Md5HashStrLenght, content);
                
        // Convert to hex-string
        stringToHexString(pMD5HashHex, md5HashStr, (const char *)"");
        String theString = pMD5HashHex;   
    }
                        
    char toSign[(strlen(canonicalResource) + 100)];
    if (useSharedKeyLite)
    {
        sprintf(toSign, "%s\%s", (char *)ptimeStamp.c_str(), canonicalResource);                        
    }
    else
    {
        sprintf(toSign, "%s\n%s\n%s\n%s\n%s", (char *)pHttpVerb.c_str(), pMD5HashHex , (char *)contentType.c_str(), (char *)ptimeStamp.c_str(), canonicalResource);                        
    }
            
    // Produce Authentication Header
    // 1) Base64 decode the Azure Storage Key
           
    // Hoe to decode Base 64 String
    //https://www.ncbi.nlm.nih.gov/IEB/ToolBox/CPP_DOC/lxr/source/src/connect/mbedtls/
            
    // Base64-decode (Azure Storage Key)
                
    char base64DecOut[80] {0};
    int decodeResult = base64_decode(_accountPtr->AccountKey.c_str(), base64DecOut);
                
    size_t decodedKeyLen = (decodeResult != -1) ? decodeResult : 0; 
                           
            /*
              volatile byte c0 = base64DecOut[0];
              volatile byte c1 = base64DecOut[1];
              volatile byte c2 = base64DecOut[2];
              volatile byte c3 = base64DecOut[3];
              volatile byte c4 = base64DecOut[4];
              volatile byte c5 = base64DecOut[5];
              volatile byte c6 = base64DecOut[6];
              volatile byte c7 = base64DecOut[7];
              volatile byte c8 = base64DecOut[8];
              volatile byte c9 = base64DecOut[1];
              volatile byte c10 = base64DecOut[2];
              volatile byte c11 = base64DecOut[3];
              volatile byte c12 = base64DecOut[0];
              volatile byte c13 = base64DecOut[1];
              volatile byte c14 = base64DecOut[2];
              volatile byte c15 = base64DecOut[3];
              volatile byte c16 = base64DecOut[0];
              volatile byte c17 = base64DecOut[1];
              volatile byte c18 = base64DecOut[2];
              volatile byte c19 = base64DecOut[3];
              volatile byte c20 = base64DecOut[0];
              volatile byte c21 = base64DecOut[1];
              volatile byte c22 = base64DecOut[2];
              volatile byte c23 = base64DecOut[3];
              volatile byte c24 = base64DecOut[0];
              volatile byte c54 = base64DecOut[54];
              volatile byte c55 = base64DecOut[55];
              volatile byte c56 = base64DecOut[56];
              volatile byte c57 = base64DecOut[57];
              volatile byte c58 = base64DecOut[58];
              volatile byte c59 = base64DecOut[59];
              volatile byte c60 = base64DecOut[60];
              volatile byte c61 = base64DecOut[61];
              volatile byte c62 = base64DecOut[62];
              volatile byte c63 = base64DecOut[63];
              volatile byte c64 = base64DecOut[64];
            */
             
    // 2) SHA-256 encode the canonical resource (Here string of: Accountname and 'Tables')
    //    with base-64 deoded Azure Storage Account Key
    // HMAC SHA-256 encoding
    // https://techtutorialsx.com/2018/01/25/esp32-arduino-applying-the-hmac-sha-256-mechanism/
            
    // create SHA256Hash           
    size_t sha256HashBufferLength = 32 + 1;
    char sha256HashStr[sha256HashBufferLength] {0};
    int create_SHA256_result = createSHA256Hash(sha256HashStr, sha256HashBufferLength, toSign, strlen(toSign), base64DecOut, decodedKeyLen); 

        /*
        volatile byte b0 = sha256HashStr[0];
        volatile byte b1 = sha256HashStr[1];
        volatile byte b2 = sha256HashStr[2];
        */
             
    // 3) Base-64 encode the SHA-265 encoded canonical resorce

    size_t resultBase64Size = 32 + 20;
    char hmacResultBase64[resultBase64Size] {0};
    base64_encode(sha256HashStr, 32, hmacResultBase64, resultBase64Size);
            
    char retBuf[_accountPtr->AccountName.length() + strlen(hmacResultBase64) + 20] {0};
    String authorizationHeader;
    if (useSharedKeyLite)
    {              
        sprintf(retBuf, "%s %s:%s", (char *)"SharedKeyLite", (char *)_accountPtr->AccountName.c_str(), hmacResultBase64);               
        authorizationHeader = retBuf;
        return authorizationHeader;
    }
    else
    {                
        sprintf(retBuf, "%s %s:%s", (char *)"SharedKey", (char *)_accountPtr->AccountName.c_str(), hmacResultBase64);                
        authorizationHeader = retBuf;
        return authorizationHeader;
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
    String theUrl = "jsonplaceholder.typicode.com";
    String thePath = "/posts?userId=1";
    
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
      Serial.println(payload);
    }
    else {
      Serial.println("Error on HTTP request");
      volatile int dummy2 = 1;
      dummy2++;
    }
}


 
az_http_status_code TableClient::CreateTable(const char * tableName, ContType pContentType, AcceptType pAcceptType, ResponseType pResponseType, bool useSharedKeyLite)
        {
            //OperationResultsClear();
            
            String timestamp = GetDateHeader();
            String timestampUTC = timestamp + ".0000000Z";
                
           
            String contentType = getContentTypeString(pContentType);
            String acceptType = getAcceptTypeString(pAcceptType);

            const char * li1 = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>";
            const char * li2 = "<entry xmlns:d=\"http://schemas.microsoft.com/ado/2007/08/dataservices\"  ";
            const char * li3 = "xmlns:m=\"http://schemas.microsoft.com/ado/2007/08/dataservices/metadata\" ";
            const char * li4 = "xmlns=\"http://www.w3.org/2005/Atom\"> <id>http://";          
                  char * li5 = (char *)_accountPtr->AccountName.c_str();
            const char * li6 = ".table.core.windows.net/Tables('";
                  char * li7 = (char *)tableName;
            const char * li8 = "')</id><title /><updated>";
                  char * li9 = (char *)timestampUTC.c_str();
            const char * li10 = "</updated><author><name/></author> ";
            const char * li11 = "<content type=\"application/xml\"><m:properties><d:TableName>";
                  char * li12 = (char *)tableName;
            const char * li13 = "</d:TableName></m:properties></content></entry>";

            
            char addBuffer[1000];
            sprintf(addBuffer, "%s%s%s%s%s%s%s%s%s%s%s%s%s", li1, li2, li3, li4,li5, li6, li7, li8, li9, li10,li11, li12, li13);
            volatile size_t theLength = strlen(addBuffer);
            String content = addBuffer;
            //String content2 = content.substring(120, 240);
            //String content3 = content.substring(230, 350);
            //String content4 = content.substring(340, 460);
            //String content5 = content.substring(450, 570);
            //delete[] addBuffer;
            volatile int dummy2 = 1;
            

            /*
            char buf[300];
            sprintf(buf,"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>" +
           "<entry xmlns:d=\"http://schemas.microsoft.com/ado/2007/08/dataservices\"  " +
           "xmlns:m=\"http://schemas.microsoft.com/ado/2007/08/dataservices/metadata\" " +
           "xmlns=\"http://www.w3.org/2005/Atom\"> " +
           "<id>http://" + _account.AccountName + ".table.core.windows.net/Tables('"
               + tableName +
           "')</id>" +
           "<title />" +
           "<updated>" + DateTime.UtcNow.ToString("yyyy-MM-ddTHH:mm:ss.0000000Z") + "</updated>" +
           "<author><name/></author> " +
           "<content type=\"application/xml\"><m:properties><d:TableName>" + tableName + "</d:TableName></m:properties></content></entry>" )
            */
            /*
            content = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>" +
           "<entry xmlns:d=\"http://schemas.microsoft.com/ado/2007/08/dataservices\"  " +
           "xmlns:m=\"http://schemas.microsoft.com/ado/2007/08/dataservices/metadata\" " +
           "xmlns=\"http://www.w3.org/2005/Atom\"> " +
           "<id>http://" + _accountPtr->AccountName + ".table.core.windows.net/Tables('"
               + tableName +
           "')</id>" +
           "<title />" +
           "<updated>" + timestamp + "</updated>" +
           "<author><name/></author> " +
           "<content type=\"application/xml\"><m:properties><d:TableName>" + tableName + "</d:TableName></m:properties></content></entry>";
           */
            
            //DateTime.UtcNow.ToString("yyyy-MM-ddTHH:mm:ss.0000000Z")
            
            



            String HttpVerb = "POST";
            String ContentMD5 = "";
            //byte hashContentMD5[] = null;
            //int contentLength = 0;
            int contentLength = strlen(addBuffer);
            
            // byte[] payload = GetBodyBytesAndLength(content, out contentLength);
            
            
            
           
            //String authorizationHeader = CreateTableAuthorizationHeader(payload, String.Format("/{0}/{1}", _account.AccountName, "Tables()"), timestamp, HttpVerb, pContentType, out ContentMD5, out hashContentMD5, useSharedKeyLite = false);
         
            char Account_Tables[100];
            sprintf(Account_Tables, "/%s/%s", (char *)_accountPtr->AccountName.c_str(), (char *)"Tables()");
            
            //char hushBuffer[1000];
            //int hashBufferLength = 1000;

            char hushBuffer[500];
            int hashBufferLength = 500;

            char md5Buffer[32 +1];

            //EncryptResults encryptResults = CreateTableAuthorizationHeader((char *)addBuffer, (char *)Account_Tables, timestamp, HttpVerb, pContentType, hushBuffer, hashBufferLength, useSharedKeyLite = false);


            String encryptResults = CreateTableAuthorizationHeader((char *)addBuffer, Account_Tables, timestamp, HttpVerb, pContentType, md5Buffer, hushBuffer, hashBufferLength, useSharedKeyLite = false);
            return AZ_HTTP_STATUS_CODE_CREATED;
            /*
            string urlPath = String.Format("{0}", tableName);
           
            string canonicalizedResource = String.Format("/{0}/{1}", _account.AccountName, urlPath);
           
            string canonicalizedHeaders = String.Format("Date:{0}\nx-ms-date:{1}\nx-ms-version:{2}", timestamp, timestamp, VersionHeader);

            string TableEndPoint = _account.UriEndpoints["Table"].ToString();

            Uri uri = new Uri(TableEndPoint + "/Tables()");

            var tableTypeHeaders = new Hashtable();
            tableTypeHeaders.Add("Accept-Charset", "UTF-8");
            tableTypeHeaders.Add("MaxDataServiceVersion", "3.0;NetFx");
            tableTypeHeaders.Add("Content-Type", contentType);
            tableTypeHeaders.Add("DataServiceVersion", "3.0");
            tableTypeHeaders.Add("Prefer", getResponseTypeString(pResponseType));
            tableTypeHeaders.Add("Content-MD5", ContentMD5);

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
        
        



       void OperationResultsClear()
        {
            //_OperationResponseETag = null;
            //_OperationResponseBody = null;
            //_OperationResponseMD5 = null;
            //_OperationResponseSingleQuery = null;
            //_OperationResponseQueryList = null;
        }
        
        
        String GetDateHeader()
        {
            //char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
 
            char daysOfTheWeek[7][4] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

            

          DateTime now = sysTime.getTime();

          String dayOfWeek = daysOfTheWeek[now.dayOfTheWeek()];
          
          char buf[42];
          
          sprintf(buf,"%04i-%02i-%02iT%02i:%02i:%02i%s", now.year() - 30, now.month(), now.day(), now.hour(), now.minute(), now.second(), ".0000000Z");
          
          //String returnValue = buf;
          String returnValue = "2020-09-30T23:31:04";

          //String returnValue =  now.timestamp(DateTime::TIMESTAMP_FULL);           
          
          return returnValue;
            //return DateTime.UtcNow.ToString("R");
        }

        String getContentTypeString(ContType pContentType)
        {
            if (pContentType == contApplicationIatomIxml)
            { return "application/atom+xml"; }
            else
            { return "application/json"; }
        }

        String getAcceptTypeString(AcceptType pAcceptType)
        {
            if (pAcceptType == acceptApplicationIatomIxml)
            { return "application/atom+xml"; }
            else
            { return "application/json"; }
        }
        /*
        byte[] GetBodyBytesAndLength(string body, out int contentLength)
        {
            var content = Encoding.UTF8.GetBytes(body);
            contentLength = content.Length;
            return content;
        }
        */