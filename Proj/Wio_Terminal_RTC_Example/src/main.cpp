// Copyright RoSchmi 2020, License Apache 2.0

// Wio_Terminal_RTC_Example

// This examples shows the function of the Realtime Clock RTC
// Some included libraries in this example are not needed for
// this purpose but the inclusion give hints for other possible
// applications (WiFiSecure, HTTPClient)

// The App fetches the time via Internet and NPT
// Then it stores the actual time in the Wio Terminal RTC
// The time can be read from the RTC from other Classes


#include <Arduino.h>
#include "WiFiUdp.h"
#include "NTP.h"
#include <WiFiClientSecure.h>
#include <RTC_SAMD51.h>
#include <DateTime.h>
#include <Time/SysTime.h>
#include <Time/TimeFetcher.h>
#include <config_secret.h>

#include "TFT_eSPI.h"
#include "Free_Fonts.h" //include the header file

#include "HTTPClient.h"

// Here is the link to the used HTTPClient
// It was added manually to the project
// https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPClient

#include "mbedtls/md.h"
#include "mbedtls/base64.h"
#include "mbedtls/sha256.h"

TFT_eSPI tft;
int current_text_line = 0;

#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define LCD_FONT FreeSans9pt7b
#define LCD_LINE_HEIGHT 18

const char *ssid = IOT_CONFIG_WIFI_SSID;
const char *password = IOT_CONFIG_WIFI_PASSWORD;

WiFiClientSecure wifi_client;

WiFiUDP wifiUdp;
NTP ntp(wifiUdp);
HTTPClient http;
HTTPClient * httpPtr = &http;
static SysTime sysTime;
TimeFetcher timeFetcher;

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

void setup() {
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);
  tft.setFreeFont(&LCD_FONT);
  tft.setTextColor(TFT_BLACK);

  pinMode(LED_BUILTIN, OUTPUT);
  lcd_log_line((char *)"Starting");

  Serial.begin(9600);
  Serial.println("\r\nHello, I'm starting ");

  char buf[50];
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
    
    lcd_log_line((char *)"> SUCCESS.");
    lcd_log_line((char*)localIpAddress.toString().c_str() );
    lcd_log_line((char*)gatewayIp.toString().c_str());
    lcd_log_line((char*)subNetMask.toString().c_str());
    lcd_log_line((char*)dnsServerIp.toString().c_str());

    Serial.println("\r\n> SUCCESS.");

    //wifi_client.setCACert(baltimore_root_ca);

     ntp.begin();
    ntp.update();
    ntp.timeZone(0,0);
    
    lcd_log_line((char *)ntp.formattedTime("%d. %B %Y"));    // dd. Mmm yyyy
    lcd_log_line((char *)ntp.formattedTime("%A %T"));        // Www hh:mm:ss

  //DateTime now = DateTime(F(__DATE__), F(__TIME__));
  DateTime now = DateTime((uint16_t) ntp.year(), (uint8_t)ntp.month(), (uint8_t)ntp.day(),
                (uint8_t)ntp.hours(), (uint8_t)ntp.minutes(), (uint8_t)ntp.seconds());

  sysTime.begin(now);

  // here we get the time through SysTime in the main method
  now = sysTime.getTime();
  sprintf(buf, "Second: %i", now.second());
  lcd_log_line(buf);
  
  // here we get the time through timeFetcher
  // this shows, that it is possible to get time in instances of other classes (TimeFetcher)
  // TimeFetcher Class uses SysTime to read RTC 
  while (true)
    {
      uint32_t startTime = millis();
      while ((millis() - startTime) < 5000)
      {}     
      now = timeFetcher.getTime();
      int theSecond = now.second();
      int theMinute = now.minute();
      int theHour = now.hour();
      sprintf(buf, "Time is: %i : %i : %i", theHour, theMinute, theSecond);
      lcd_log_line(buf);
    }
}

void loop() {
  // put your main code here, to run repeatedly:
}