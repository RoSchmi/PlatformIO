// Working example shows debugging with JLink on Seeed Wio Terminal
// There are some hacks needed to get it working
// See remarks here an in platformio.ini

// For debugging these commands in arduino/main.cpp have to be commented out
// For normal upload without debugging the commands must be activated

// #elif defined(USBCON)
//  //USBDevice.init();
//  //USBDevice.attach();
// #endif

#include <Arduino.h>
#include "TFT_eSPI.h"
#include "Free_Fonts.h" //include the header file
TFT_eSPI tft;
int current_text_line = 0;
#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define LCD_FONT FreeSans9pt7b
#define LCD_LINE_HEIGHT 18


volatile int counter = 0;
volatile int counter2 = 0;
char strData[100];

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
  lcd_log_line("Starting");
  
  Serial.begin(9600);
  Serial.println("\r\nHello, I'm starting ");
}

void loop() {

  for (int i = 0; i < 5; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    
    delay(500);
    sprintf(strData, "Inner Loop %u", i);   
    lcd_log_line(strData);
    Serial.print("Inner Loop ");
    Serial.println(i);
  }
    sprintf(strData, "   Outer Loop %u", counter);   
    lcd_log_line(strData);
    Serial.print("    Outer Loop ");
    Serial.println(counter);
    counter++;
}

