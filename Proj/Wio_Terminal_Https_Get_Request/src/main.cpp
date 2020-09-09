// 'Wio_Terminal_Https_Request'
// Copyright RoSchmi 2020, License Apache 2.0
//
// Example of secure (https) requests for the Seeed Wio Terminal
// The arduino-esp32 HTTPClient was used
// https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPClient

/*
For debugging these commands in arduino/main.cpp have to be commented out
For normal upload without debugging the commands must be activated

#elif defined(USBCON)
  //USBDevice.init();
  //USBDevice.attach();
#endif

Don't forget to restore when not debugging otherwise
the console output will not work
*/

#include <Arduino.h>

#include <WiFiClientSecure.h>
#include "WiFiUdp.h"
#include "NTP.h"
#include "HTTPClient.h"
// Here is the link to the used HTTPClient
// It was added manually to the project
// https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPClient

#include "mbedtls/md.h"
#include "mbedtls/base64.h"
#include "mbedtls/sha256.h"

#include "TFT_eSPI.h"
#include "Free_Fonts.h"    //include the header file
TFT_eSPI tft;

#include "config.h"    // Contains WiFi Credentials

int current_text_line = 0;
#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define LCD_FONT FreeSans9pt7b
#define LCD_LINE_HEIGHT 18

#define TIMEZONE 1.0      // TimeZone time difference to UTC

volatile int counter = 0;
volatile int counter2 = 0;
char strData[100];

const char *ssid = IOT_CONFIG_WIFI_SSID;
const char *password = IOT_CONFIG_WIFI_PASSWORD;

const char *baltimore_root_ca =
"-----BEGIN CERTIFICATE-----\n"
"MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"
"RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"
"VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"
"DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"
"ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"
"VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"
"mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"
"IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"
"mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"
"XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"
"dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"
"jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"
"BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"
"DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"
"9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"
"jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"
"Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"
"ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"
"R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"
"-----END CERTIFICATE-----";

WiFiClientSecure wifi_client;
WiFiUDP wifiUdp;
NTP ntp(wifiUdp);
HTTPClient http;


// Forward declarations:
void lcd_log_line(char* line);

void setup() { 
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);
  tft.setFreeFont(&LCD_FONT);
  tft.setTextColor(TFT_BLACK);

  pinMode(LED_BUILTIN, OUTPUT);
  lcd_log_line("Starting");
  
  Serial.begin(9600);
  Serial.println("\r\nHello, I'm starting ");

  char buf[42];
    sprintf(buf, "Connecting to SSID: %s", ssid);
    lcd_log_line(buf);
    Serial.println(buf);
    //WiFi.enableIpV6();
    WiFi.begin(ssid, password);

    // attempt to connect to Wifi network:
        
    while((WiFi.status() != WL_CONNECTED))
    {  
      lcd_log_line(itoa((int)WiFi.status(), buf, 10));
        Serial.print(".");
        // wait 1 second for re-trying
        delay(1000);        
    }
    IPAddress localIpAddress = WiFi.localIP();
    IPAddress gatewayIp =  WiFi.gatewayIP();
    IPAddress subNetMask =  WiFi.subnetMask();
    IPAddress dnsServerIp = WiFi.dnsIP();
    
    lcd_log_line("> SUCCESS.");
    lcd_log_line((char*)localIpAddress.toString().c_str() );
    lcd_log_line((char*)gatewayIp.toString().c_str());
    lcd_log_line((char*)subNetMask.toString().c_str());
    lcd_log_line((char*)dnsServerIp.toString().c_str());

    Serial.println("\r\n> SUCCESS.");

    wifi_client.setCACert(baltimore_root_ca);
    
    ntp.begin();
    ntp.update();
    ntp.timeZone(TIMEZONE);  
    
    lcd_log_line(ntp.formattedTime("%d. %B %Y")); // dd. Mmm yyyy
    lcd_log_line(ntp.formattedTime("%A %T"));

    //http.begin(wifi_client, "https://jsonplaceholder.typicode.com/posts?userId=1");
    http.begin(wifi_client, "jsonplaceholder.typicode.com", 443, "/posts?userId=1", true);
    int httpCode = http.GET();

    if (httpCode > 0) { //Check for the returning code 
      String payload = http.getString();
      lcd_log_line("Http-Code:");
      lcd_log_line(itoa((int)httpCode, buf, 10));
      int length = payload.length();
      int indexCtr = 0;
      int pageWidth = 30;
      Serial.println(httpCode);
      delay(2000);
      while (indexCtr < length)
      {
        lcd_log_line((char*)payload.substring(indexCtr, indexCtr + pageWidth).c_str());
        indexCtr += pageWidth;
      } 
      Serial.println(payload);
    }
    else {
      Serial.println("Error on HTTP request");
    }

    delay(3000);
}

void loop() {
  // Blink a LED, that's awesome!
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
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





