// Working Example
// Platformio Development and Debugging Example for BrainPad BP2
// SWD-Debugging with Segger J-Link Edu

// Board Selection is made for Nucleo STM32F401RE
// The initialization of the board leads to an exception in
// stm32_def.c where the program hangs in a While Loop

// "WEAK void _Error_Handler(const char *msg, int val)"

// As a hotfix the while loop is commented out

// Blinks the Blue LED, when Button U is pressed blinks the red LED

#include <Arduino.h>

volatile int theCounter = 0;
bool state = LOW;

void setup() {
  
  pinMode(34, OUTPUT); // Red
  pinMode(33, OUTPUT); // Green
  pinMode(32, OUTPUT); // Blue

   pinMode(13, INPUT_PULLUP);   // Button U = 13

  //Serial.begin(115200);
}

void loop() {
  state = digitalRead(13);
  
  if (state == HIGH)
  {
    digitalWrite(34, HIGH);   // turn the red LED on 
    delay(1000);               // wait for a time
    digitalWrite(34, LOW);    // turn the red LED off 
    delay(1000);               // wait for a time
  }
  else
  {  
    digitalWrite(32, HIGH);   // turn the blue LED on   
    delay(500);               // wait for a time
    digitalWrite(32, LOW);    // turn the blue LED off  
    delay(500);               // wait for a time    
  }
  theCounter++;
}

