#include <AzureStorage/CloudStorageAccount.h>
#include <HTTPClient.h>
#include <AzureStorage/TableClient.h>
#include <DateTime.h>


String VersionHeader = "2015-04-05";

CloudStorageAccount  * _accountPtr;
HTTPClient * _httpPtr;
const char * _caCert;

static SysTime sysTime;

        char * _OperationResponseBody = "";
        char * _OperationResponseMD5 = "";
        char * _OperationResponseETag = "";

        //private Hashtable _OperationResponseSingleQuery = null;
        //private ArrayList _OperationResponseQueryList = null;



// forward declarations
String GetDateHeader();

//DateTime myDateTime;



TableClient::TableClient(CloudStorageAccount *account, const char * caCert, HTTPClient *httpClient)
{
    _accountPtr = account;
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

            return AZ_HTTP_STATUS_CODE_CREATED;

            /*
            string content = string.Empty;

            string contentType = getContentTypeString(pContentType);
            string acceptType = getAcceptTypeString(pAcceptType);

            //long totalMemory = GC.GetTotalMemory(true);
        
            content = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>" +
           "<entry xmlns:d=\"http://schemas.microsoft.com/ado/2007/08/dataservices\"  " +
           "xmlns:m=\"http://schemas.microsoft.com/ado/2007/08/dataservices/metadata\" " +
           "xmlns=\"http://www.w3.org/2005/Atom\"> " +
           "<id>http://" + _account.AccountName + ".table.core.windows.net/Tables('"
               + tableName +
           "')</id>" +
           "<title />" +
           "<updated>" + DateTime.UtcNow.ToString("yyyy-MM-ddTHH:mm:ss.0000000Z") + "</updated>" +
           "<author><name/></author> " +
           "<content type=\"application/xml\"><m:properties><d:TableName>" + tableName + "</d:TableName></m:properties></content></entry>";


            string HttpVerb = "POST";
            string ContentMD5 = string.Empty;
            byte[] hashContentMD5 = null;
            int contentLength = 0;
            byte[] payload = GetBodyBytesAndLength(content, out contentLength);

            content = null; // free memory
           
            string authorizationHeader = CreateTableAuthorizationHeader(payload, String.Format("/{0}/{1}", _account.AccountName, "Tables()"), timestamp, HttpVerb, pContentType, out ContentMD5, out hashContentMD5, useSharedKeyLite = false);
         
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

          DateTime now = sysTime.getTime();
          
          //char buf[42];
          //sprintf(buf,"%i:%i:%i-%i:%i:%i", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
           String returnValue =  now.timestamp();           
          
          return returnValue;
            //return DateTime.UtcNow.ToString("R");
        }
        
        