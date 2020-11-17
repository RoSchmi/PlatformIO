
// This example uses the new Seeed eRPC WiFi library

//https://wiki.seeedstudio.com/Wio-Terminal-Reading-Github/

#include <Arduino.h>
#include "rpcWiFi.h"
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "Free_Fonts.h"
#include "TFT_eSPI.h"
#include <config_secret.h>
TFT_eSPI tft;

int current_text_line = 0;

#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define LCD_FONT FreeSans9pt7b
#define LCD_LINE_HEIGHT 18
 
const char* ssid     = IOT_CONFIG_WIFI_SSID;
const char* password = IOT_CONFIG_WIFI_PASSWORD;


//29*27
const unsigned char watchIMG[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 
  0x00, 0xFC, 0x07, 0x00, 0x00, 0xFF, 0x1F, 0x00, 0x80, 0x1F, 0x3F, 0x00, 
  0xC0, 0x07, 0x7C, 0x00, 0xE0, 0x03, 0xF8, 0x00, 0xF0, 0xE1, 0xF0, 0x01, 
  0xF8, 0xF1, 0xF1, 0x01, 0xF8, 0xF8, 0xF1, 0x03, 0xFC, 0xF8, 0xF1, 0x03, 
  0xF8, 0xF0, 0xF1, 0x03, 0xF0, 0xF1, 0xF0, 0x01, 0xF0, 0x01, 0xF8, 0x00, 
  0xE0, 0x03, 0x78, 0x00, 0xC0, 0x07, 0x3E, 0x00, 0x80, 0xFF, 0x1F, 0x00, 
  0x00, 0xFE, 0x0F, 0x00, 0x00, 0xF8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  };
 
const unsigned char starIMG[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 
  0x00, 0xC0, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0xE0, 0x01, 0x00, 
  0x00, 0xF0, 0x01, 0x00, 0x00, 0xF0, 0x03, 0x00, 0x00, 0xF8, 0x03, 0x00, 
  0x00, 0xF8, 0x07, 0x00, 0x00, 0xFE, 0x0F, 0x00, 0xFE, 0xFF, 0xFF, 0x1F, 
  0xFE, 0xFF, 0xFF, 0x0F, 0xFC, 0xFF, 0xFF, 0x07, 0xF8, 0xFF, 0xFF, 0x03, 
  0xF0, 0xFF, 0xFF, 0x01, 0xC0, 0xFF, 0xFF, 0x00, 0x80, 0xFF, 0x7F, 0x00, 
  0x00, 0xFF, 0x3F, 0x00, 0x80, 0xFF, 0x3F, 0x00, 0x80, 0xFF, 0x3F, 0x00, 
  0x80, 0xFF, 0x3F, 0x00, 0x80, 0xFF, 0x7F, 0x00, 0x80, 0xBF, 0x7F, 0x00, 
  0xC0, 0x1F, 0x7E, 0x00, 0xC0, 0x07, 0x78, 0x00, 0xC0, 0x01, 0x70, 0x00, 
  0x40, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  };
 
const unsigned char forkIMG[] = {
  0xF8, 0x00, 0x00, 0x00, 0xFE, 0x03, 0x00, 0x00, 0x0F, 0x07, 0x00, 0x01, 
  0x07, 0x07, 0xE0, 0x0F, 0x07, 0x07, 0x78, 0x1E, 0x9E, 0x07, 0x38, 0x38, 
  0xFC, 0x01, 0x38, 0x38, 0xF8, 0x00, 0x38, 0x3C, 0xF8, 0x00, 0xF0, 0x1F, 
  0xF8, 0x00, 0xE0, 0x07, 0xF8, 0x00, 0xC0, 0x07, 0xF8, 0x00, 0xE0, 0x07, 
  0xF8, 0x00, 0xE0, 0x07, 0xF8, 0x00, 0xF8, 0x03, 0xF8, 0x00, 0xFE, 0x01, 
  0xF8, 0xE0, 0xFF, 0x00, 0xF8, 0xFF, 0x7F, 0x00, 0xF8, 0xFF, 0x1F, 0x00, 
  0xF8, 0xFF, 0x03, 0x00, 0xF8, 0x1F, 0x00, 0x00, 0xF8, 0x03, 0x00, 0x00, 
  0xF8, 0x01, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0xFC, 0x01, 0x00, 0x00, 
  0x9E, 0x07, 0x00, 0x00, 0x07, 0x07, 0x00, 0x00, 0x07, 0x07, 0x00, 0x00, 
  0x0F, 0x07, 0x00, 0x00, 0xFE, 0x03, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 
  };
 
const char*  server = "api.github.com";  // Server URL

const char* test_root_ca = \
                            "-----BEGIN CERTIFICATE-----\n"
                            "MIIEsTCCA5mgAwIBAgIQBOHnpNxc8vNtwCtCuF0VnzANBgkqhkiG9w0BAQsFADBs\n"
                            "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
                            "d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n"
                            "ZSBFViBSb290IENBMB4XDTEzMTAyMjEyMDAwMFoXDTI4MTAyMjEyMDAwMFowcDEL\n"
                            "MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n"
                            "LmRpZ2ljZXJ0LmNvbTEvMC0GA1UEAxMmRGlnaUNlcnQgU0hBMiBIaWdoIEFzc3Vy\n"
                            "YW5jZSBTZXJ2ZXIgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC2\n"
                            "4C/CJAbIbQRf1+8KZAayfSImZRauQkCbztyfn3YHPsMwVYcZuU+UDlqUH1VWtMIC\n"
                            "Kq/QmO4LQNfE0DtyyBSe75CxEamu0si4QzrZCwvV1ZX1QK/IHe1NnF9Xt4ZQaJn1\n"
                            "itrSxwUfqJfJ3KSxgoQtxq2lnMcZgqaFD15EWCo3j/018QsIJzJa9buLnqS9UdAn\n"
                            "4t07QjOjBSjEuyjMmqwrIw14xnvmXnG3Sj4I+4G3FhahnSMSTeXXkgisdaScus0X\n"
                            "sh5ENWV/UyU50RwKmmMbGZJ0aAo3wsJSSMs5WqK24V3B3aAguCGikyZvFEohQcft\n"
                            "bZvySC/zA/WiaJJTL17jAgMBAAGjggFJMIIBRTASBgNVHRMBAf8ECDAGAQH/AgEA\n"
                            "MA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIw\n"
                            "NAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2Vy\n"
                            "dC5jb20wSwYDVR0fBEQwQjBAoD6gPIY6aHR0cDovL2NybDQuZGlnaWNlcnQuY29t\n"
                            "L0RpZ2lDZXJ0SGlnaEFzc3VyYW5jZUVWUm9vdENBLmNybDA9BgNVHSAENjA0MDIG\n"
                            "BFUdIAAwKjAoBggrBgEFBQcCARYcaHR0cHM6Ly93d3cuZGlnaWNlcnQuY29tL0NQ\n"
                            "UzAdBgNVHQ4EFgQUUWj/kK8CB3U8zNllZGKiErhZcjswHwYDVR0jBBgwFoAUsT7D\n"
                            "aQP4v0cB1JgmGggC72NkK8MwDQYJKoZIhvcNAQELBQADggEBABiKlYkD5m3fXPwd\n"
                            "aOpKj4PWUS+Na0QWnqxj9dJubISZi6qBcYRb7TROsLd5kinMLYBq8I4g4Xmk/gNH\n"
                            "E+r1hspZcX30BJZr01lYPf7TMSVcGDiEo+afgv2MW5gxTs14nhr9hctJqvIni5ly\n"
                            "/D6q1UEL2tU2ob8cbkdJf17ZSHwD2f2LSaCYJkJA69aSEaRkCldUxPUd1gJea6zu\n"
                            "xICaEnL6VpPX/78whQYwvwt/Tv9XBZ0k7YXDK/umdaisLRbvfXknsuvCnQsH6qqF\n"
                            "0wGjIChBWUMo0oHjqvbsezt3tkBigAVBRQHvFwY+3sAzm2fTYS5yh+Rp/BIAV0Ae\n"
                            "cPUeybQ=\n"
                            "-----END CERTIFICATE-----\n";

// You can use x.509 client certificates if you want
//const char* test_client_key = "";   //to verify the client
//const char* test_client_cert = "";  //to verify the client

String data;
 
WiFiClientSecure client;

String github_data;
 
// forward declarations
void lcd_log_line(char* line);
void getData(void);

void setup() {
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_WHITE);
    tft.setFreeFont(&LCD_FONT);
    tft.setTextColor(TFT_BLACK);

    pinMode(LED_BUILTIN, OUTPUT);
    lcd_log_line((char *)"Starting");
  
    //Initialize serial and wait for port to open:
    Serial.begin(115200);
    delay(100);
 
    pinMode(WIO_KEY_C, INPUT_PULLUP);
 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);

    char buf[100];
    lcd_log_line((char *)"Initial WiFi-Status:");
    lcd_log_line(itoa((int)WiFi.status(), buf, 10));
    
    delay(2000);
    //Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    lcd_log_line((char *)"First disconnecting, Status:");
    lcd_log_line(itoa((int)WiFi.status(), buf, 10));
    
    sprintf(buf, "Connecting to SSID: %s", ssid);
    lcd_log_line(buf);

    if (!ssid || *ssid == 0x00 || strlen(ssid) > 31)
    {
      // Stay in endless loop
        lcd_log_line((char *)"Invalid: SSID or PWD");
        delay(1000);
    }

    WiFi.begin(ssid, password);
    
    if (!WiFi.enableSTA(true))
    {
      while (true)
      {
        // Stay in endless loop
        lcd_log_line((char *)"Connect failed.");
        delay(1000);
      }
    }
    
    lcd_log_line((char *)"Connected, new Status:");
    lcd_log_line(itoa((int)WiFi.status(), buf, 10));
    
    IPAddress localIpAddress = WiFi.localIP();
    IPAddress gatewayIp =  WiFi.gatewayIP();
    IPAddress subNetMask =  WiFi.subnetMask();
    IPAddress dnsServerIp = WiFi.dnsIP(1);

    delay(3000);
    current_text_line = 0;
    tft.fillScreen(TFT_WHITE);
    
    lcd_log_line((char *)"> SUCCESS.");
    sprintf(buf, "Ip: %s", (char*)localIpAddress.toString().c_str());
    lcd_log_line(buf);
    sprintf(buf, "Gateway: %s", (char*)gatewayIp.toString().c_str());
    lcd_log_line(buf);
    sprintf(buf, "Subnet: %s", (char*)subNetMask.toString().c_str());
    lcd_log_line(buf);
    sprintf(buf, "DNS-Server: %s", (char*)dnsServerIp.toString().c_str());
    lcd_log_line(buf);
  
   
    delay(10000);
  
    Serial.print("Connected to ");
    Serial.println(ssid);
 
    tft.fillScreen(TFT_BLACK);
    tft.setCursor((320 - tft.textWidth("Connected!"))/2, 120);
    tft.print("Connected!");
 
    getData();
}
 
int lastState = 1;
void loop()
{
    int currentState = digitalRead(WIO_KEY_C);
    if (currentState == 0) {
      if (currentState != lastState) {
        getData();
      }
      lastState = !lastState;
    }
}

void getData() {
    client.setCACert(test_root_ca);
  //client.setCertificate(test_client_key); // for client verification
  //client.setPrivateKey(test_client_cert); // for client verification
 
    tft.fillScreen(TFT_BLACK);
    tft.setCursor((320 - tft.textWidth("Conectting to Server.."))/2, 120);
    tft.print("Connecting to Server..");
 
    Serial.println("\nStarting connection to server...");
    if (!client.connect(server, 443)) {
        Serial.println("Connection failed!");
        tft.fillScreen(TFT_BLACK);
        tft.setCursor((320 - tft.textWidth("Connection failed!"))/2, 120);
        tft.print("Connection failed!");
    } else {
        Serial.println("Connected to server!");
        tft.fillScreen(TFT_BLACK);
        tft.setCursor((320 - tft.textWidth("Connected to Server!"))/2, 120);
        tft.print("Connected to Server!");
 
        // Make a HTTP request:
        client.println("GET https://api.github.com/repos/micropython/micropython HTTP/1.0");
        client.println("Host: api.github.com");
        client.println("User-Agent: Seeed-Studio");
        client.println("Connection: close");
        client.println();
 
        while (client.connected()) {
            String line = client.readStringUntil('\n');
            if (line == "\r") {
                Serial.println("headers received");
                break;
            }
        }
 
        while(client.available())
        {
          String line = client.readStringUntil('\r');
          github_data = line;
        }
        //Serial.println(github_data);
        client.stop();
        Serial.println("closing connection");          
    }
 
    //ArduinoJson to parse data, plesae check ArduinoJson for more info
    const size_t capacity = 2*JSON_OBJECT_SIZE(18) + JSON_OBJECT_SIZE(77) + 6050;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, github_data);
 
    int subscribers_count = doc["subscribers_count"];
    int forks = doc["forks"];
    int stargazers_count = doc["stargazers_count"];
 
    Serial.println(subscribers_count);
    Serial.println(forks);
    Serial.println(stargazers_count);
 
    //TFT SCREEN SESSION
    tft.setFreeFont(FSS9);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.fillScreen(TFT_BLACK);
    tft.drawString("MicroPython Github",20,10);
    tft.drawFastHLine(185,18,120 ,TFT_RED);
    tft.drawFastHLine(185,19,120 ,TFT_RED);
    tft.drawFastHLine(185,20,120 ,TFT_RED);
    tft.drawFastHLine(10,33,300 ,TFT_NAVY);
    tft.drawFastHLine(10,34,300 ,TFT_NAVY);
    tft.drawFastHLine(10,35,300 ,TFT_NAVY);
 
    tft.drawXBitmap(20,60, watchIMG, 29, 27, TFT_WHITE);
    tft.drawXBitmap(20,120, starIMG, 30, 30, TFT_WHITE);
    tft.drawXBitmap(20,180, forkIMG, 30, 30, TFT_WHITE);
 
    tft.setFreeFont(FMB9);
    tft.drawString("Watchers: ",60, 68);
    tft.drawString("Stars: ",60, 128);
    tft.drawString("Forks: ",60, 188);
 
    tft.drawNumber(subscribers_count, 165, 68);
    tft.drawNumber(stargazers_count, 130, 128);
    tft.drawNumber(forks, 130, 188);
}

void lcd_log_line(char* line) {
    // clear line
    tft.fillRect(0, current_text_line * LCD_LINE_HEIGHT, 320, LCD_LINE_HEIGHT, TFT_WHITE);
    tft.drawString(line, 5, current_text_line * LCD_LINE_HEIGHT);

    current_text_line++;
    current_text_line %= ((LCD_HEIGHT-20)/LCD_LINE_HEIGHT);
    if (current_text_line == 0) {
      tft.fillScreen(TFT_WHITE);
    }
}