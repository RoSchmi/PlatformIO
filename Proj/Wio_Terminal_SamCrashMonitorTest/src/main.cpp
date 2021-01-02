// Adaption of https://github.com/cyrusbuilt/SAMCrashMonitor
// to Wio Terminal and PlatformIo


#include <Arduino.h>
#include "SAMCrashMonitor.h"

#include "TFT_eSPI.h"
#include "Free_Fonts.h" //include the header file

TFT_eSPI tft;
int current_text_line = 0;

#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define LCD_FONT FreeSans9pt7b
#define LCD_LINE_HEIGHT 18

bool ledState = false;
uint8_t lastResetCause = -1;
String lastResetDescription = "";

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
    lcd_log_line((char *)"Starting... wait 6 sec");
    lcd_log_line((char *)"Disabling watchdog.");
  
    Serial.begin(9600);
    Serial.println("\r\nStarting... wait 6 sec");

    SAMCrashMonitor::begin();
    lastResetCause = SAMCrashMonitor::getResetCause();
    lastResetDescription =  SAMCrashMonitor::getResetDescription();
    
    SAMCrashMonitor::dump();
    SAMCrashMonitor::disableWatchdog();
    lcd_log_line((char *)"Last ResetCause:");
    lcd_log_line((char *)lastResetDescription.c_str());

    delay(3000);
 
    Serial.println();
    
    Serial.println();
    
    lcd_log_line((char *)"Enabling watchdog.");
    Serial.println(F("Enabling watchdog."));
    int timeout = SAMCrashMonitor::enableWatchdog(4000);

    char buf[100];
    sprintf(buf, "Watchdog enabled: %i %s", timeout, "ms");
    lcd_log_line(buf);
    lcd_log_line((char *)"Waiting for Reset...");
    Serial.print(F("Watchdog enabled for "));
    Serial.print(timeout);
    Serial.println(" ms.");
    
    Serial.println();
    Serial.println(F("First test: Looping once per second for 5 seconds while feeding watchdog..."));
    for (int i = 1; i <= 5; i++) {
        Serial.print(F("Loop #"));
        Serial.println(i);
        delay(1000);
        SAMCrashMonitor::iAmAlive();
    }

    Serial.println();
    Serial.println(F("Disabling watchdog..."));
    Serial.println();
    SAMCrashMonitor::disableWatchdog();

    Serial.println(F("Second test: Exceed timeout and reset."));
    timeout = SAMCrashMonitor::enableWatchdog(4000);
    Serial.print(F("Watchdog will reset controller in "));
    Serial.print(timeout);
    Serial.println(" ms!");
    delay(timeout + 1000);

    // We shouldn't get this far since the watchdog should reset the MCU.
}

void loop() {
    // Normally, you would place a call to SAMCrashMonitor::iAmAlive() in here.
}