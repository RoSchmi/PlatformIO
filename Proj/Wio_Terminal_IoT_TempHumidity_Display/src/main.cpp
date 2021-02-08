// This is a modification of the hackster.io project 'WioTerminal IoTs Temp&Humi display'
// https://www.hackster.io/SeeedStudio/wioterminal-iots-temp-humi-display-966426
// 

#include <Arduino.h>
#include "defines.h"
#include "DHT.h"
#include "SinricPro_Generic.h"
#include "SinricProTemperaturesensor.h"
#include <SPI.h>
#include "TFT_eSPI.h"                
#include "Free_Fonts.h"
#include "Free_Fonts.h"

TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft);  // Sprite
TFT_eSprite spr2 = TFT_eSprite(&tft);  // Sprite

#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define LCD_FONT FreeSans9pt7b
#define LCD_LINE_HEIGHT 18

const GFXfont *textFont = FSSB9;

uint32_t screenColor = TFT_BLACK;

uint32_t backColor = TFT_WHITE;
uint32_t textColor = TFT_BLACK;

#define EVENT_WAIT_TIME 5000

#define USING_LCD     false
#if USING_LCD
#include <LovyanGFX.hpp>              // https://github.com/lovyan03/LovyanGFX
LGFX lcd;
LGFX_Sprite spr = LGFX_Sprite(&lcd);
#define tft lcd
#else

#endif

DHT dht(0, DHT22);

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

bool  deviceIsOn;                               // Temeprature sensor on/off state
float temperature = 0;                              // actual temperature
float humidity = 0;                                 // actual humidity
float lastTemperature;                          // last known temperature (for compare)
float lastHumidity;                             // last known humidity (for compare)
unsigned long lastEvent = (-EVENT_WAIT_TIME);   // last time event has been sent

bool onPowerState(const String &deviceId, bool &state)
{
  Serial.println("TemperatureSensor turned " + String(state ? "on" : "off"));
  deviceIsOn = state; // turn on / off temperature sensor
  return true; // request handled properly
}

void handleTemperaturesensor()
{
  // device is off...do nothing
  if (deviceIsOn == false)
    return;

  unsigned long actualMillis = millis();

  if (actualMillis - lastEvent < EVENT_WAIT_TIME)
    return; //only check every EVENT_WAIT_TIME milliseconds

  temperature = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //  temperature = dht.readTemperature(true);
  humidity = dht.readHumidity();                          // get actual humidity

  // Serial.print("1111111111");
  // Serial.println(temperature);
  // Serial.println(humidity);
  // Serial.print("22222222222");

    spr2.fillSprite(TFT_BLACK);
    spr2.createSprite(280, 60);
    spr2.setFreeFont(&FreeSansBoldOblique12pt7b);
    spr2.setTextColor(TFT_WHITE);
    spr2.drawNumber(temperature, 40 , 0);
    spr2.drawNumber(humidity, 220 , 0);
    spr2.pushSprite(0, 110);


  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawNumber(temperature, 40 , 110, 4);
  tft.drawNumber(humidity, 220 , 110, 4);



  if (isnan(temperature) || isnan(humidity))
  {
    // reading failed...
    Serial.println("DHT reading failed");       // print error message
    return;                                     // try again next time
  }

  // Check if any reads failed and exit early (to try again).
  if (temperature == lastTemperature || humidity == lastHumidity)
    return;

  //SinricProTemperaturesensor &mySensor = SinricPro["DEVICE ID"];     // get temperaturesensor device
  SinricProTemperaturesensor &mySensor = SinricPro[TEMPERATURESENSOR_ID];     // get temperaturesensor device

  
  bool success = mySensor.sendTemperatureEvent(temperature, humidity);  // send event

  if (success)
  {
    // if event was sent successfuly, print temperature and humidity to serial
    Serial.println("Temperature: " + String(temperature, 1) + " Celsius\tHumidity: " + String(humidity, 1) + " %");
  }
  else
  {
    // if sending event failed, print error message
    Serial.println("Something went wrong...Could not send Event to server!");
  }

  lastTemperature = temperature;  // save actual temperature for next compare
  lastHumidity = humidity;        // save actual humidity for next compare
  lastEvent = actualMillis;       // save actual time for next compare


}


// setup function for WiFi connection
void setupWiFi()
{
  

//Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  Serial.println((char *)"First disconnecting.");
  while (WiFi.status() != WL_DISCONNECTED)
  {
    WiFi.disconnect();
    delay(200); 
  }
  
  delay(500);
  Serial.println((char *)"First disconnecting.");
  char buf[100];
  sprintf(buf, "Connecting to SSID: %s", ssid);
  Serial.println(buf);
  
  if (!ssid || *ssid == 0x00 || strlen(ssid) > 31)
  {
    Serial.println((char *)"Invalid: SSID or PWD, Stop");
    // Stay in endless loop
      while (true)
      {         
        delay(1000);
      }
  }

  Serial.print("\nssid: Connecting");

  WiFi.begin(ssid, password);

  if (!WiFi.enableSTA(true))
  {
  
    while (true)
    {
      // Stay in endless loop to reboot through Watchdog
      Serial.println((char *)"Connect failed.");
      delay(1000);
    }
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(250);
    WiFi.begin(ssid, password);
  }
  Serial.print("[WiFi]: IP-Address is ");
  Serial.println(WiFi.localIP());
}

// setup function for SinricPro
void setupSinricPro()
{
  // add device to SinricPro
  SinricProTemperaturesensor &mySensor = SinricPro[TEMPERATURESENSOR_ID];
  mySensor.onPowerState(onPowerState);

  // setup SinricPro
  SinricPro.onConnected([]()
  {
    Serial.println("Connected to SinricPro");

    //    spr.createSprite(320, 50);
    spr.fillSprite(TFT_BLACK);
    spr.setFreeFont(&FreeSansBoldOblique18pt7b);
    spr.setTextColor(TFT_BLUE);
    spr.drawString("Temp&humi IoTs", 60 - 40, 10 , 1);// Print the test text in the custom font
    for (int8_t line_index = 0; line_index < 5 ; line_index++)
    {
      spr.drawLine(0, 50 + line_index, 320, 50 + line_index, TFT_GREEN);
    }
    spr.setFreeFont(&FreeSansBoldOblique9pt7b);
    spr.setTextColor(TFT_GREEN);
    spr.drawString("Temp:", 60 - 24, 100 - 24 , 1); // Print the test text in the custom font
    spr.drawRoundRect(60 - 24, 100, 80, 40, 5, TFT_WHITE);
    spr.setTextColor(TFT_WHITE);
    //  spr.drawNumber(val, 60 - 20, 100 + 10, 1);
    spr.setTextColor(TFT_GREEN);
    spr.drawString("C", 60 + 32, 100 + 8, 1);

    spr.drawString("Humi:", 230 - 24 , 100 - 24 , 1); // Print the test text in the custom font
    spr.drawRoundRect(230 - 24, 100, 80, 40, 5, TFT_WHITE);
    spr.setTextColor(TFT_WHITE);
    //  spr.drawNumber(val, 230 - 20, 100 + 10, 1);
    spr.setTextColor(TFT_GREEN);
    spr.drawString("%", 230 + 32, 100 + 8, 1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawNumber(temperature, 40 , 110, 4);
    tft.drawNumber(humidity, 220 , 110, 4);
    spr.pushSprite(0, 0);

  });

  SinricPro.onDisconnected([]()
  {
    Serial.println("Disconnected from SinricPro");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawNumber(temperature, 40 , 110, 4);
    tft.drawNumber(humidity, 220 , 110, 4);
  });

  SinricPro.begin(APP_KEY, APP_SECRET);

  SinricPro.restoreDeviceStates(true); // get latest known deviceState from server (is device turned on?)
}

// main setup function
void setup()
{
  tft.begin();
  //tft.init();
  tft.setRotation(3);
  tft.fillScreen(screenColor);
  tft.setFreeFont(&LCD_FONT);
  tft.setTextColor(TFT_BLACK);

  Serial.begin(BAUD_RATE);
  //  while (!Serial);

  Serial.println("\nStarting WIO_Terminal_TemperatureSensor on " + String(BOARD_NAME));
  Serial.println("Version : " + String(SINRICPRO_VERSION_STR));

  dht.begin();

  
  spr.createSprite(tft.width(), tft.height());
  spr.createSprite(320, 140);
  spr.fillSprite(TFT_BLACK);
  spr.setFreeFont(&FreeSansBoldOblique18pt7b);
  spr.setTextColor(TFT_BLUE);
  spr.drawString("Temp&humi IoTs", 60 - 40, 10 , 1);// Print the test text in the custom font
  for (int8_t line_index = 0; line_index < 5 ; line_index++)
  {
    spr.drawLine(0, 50 + line_index, 320, 50 + line_index, TFT_RED);
  }

  spr.setFreeFont(&FreeSansBoldOblique9pt7b);
  spr.setTextColor(TFT_GREEN);
  spr.drawString("Temp:", 60 - 24, 100 - 24 , 1); // Print the test text in the custom font
  spr.drawRoundRect(60 - 24, 100, 80, 40, 5, TFT_WHITE);
  spr.setTextColor(TFT_WHITE);
  //spr.drawNumber(val, 60 - 20, 100 + 10, 1);
  spr.setTextColor(TFT_GREEN);
  spr.drawString("C", 60 + 32, 100 + 8, 1);

  spr.drawString("Humi:", 230 - 24 , 100 - 24 , 1); // Print the test text in the custom font
  spr.drawRoundRect(230 - 24, 100, 80, 40, 5, TFT_WHITE);
  spr.setTextColor(TFT_WHITE);
  //  spr.drawNumber(val, 230 - 20, 100 + 10, 1);
  spr.setTextColor(TFT_GREEN);
  spr.drawString("%", 230 + 32, 100 + 8, 1);

  spr.pushSprite(0, 0);


  setupWiFi();
  setupSinricPro();
}

void loop()
{
  SinricPro.handle();
  handleTemperaturesensor();
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawNumber(lastTemperature, 40 , 110, 4);
  tft.drawNumber(lastHumidity, 220 , 110, 4);
}