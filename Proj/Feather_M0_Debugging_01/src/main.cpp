// Simple project to develop Arduino code for Feather M0 on PlatformIo
// using j-Link EDU Emulator for Debugging
// See platformio.ini to see the presets

#include <Arduino.h>

uint8_t myVariable = 0x00;

void setup() {
  // put your setup code here, to run once:

  pinMode(13,OUTPUT);

  //Serial.begin(9600);
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(13, HIGH);
  myVariable = 0xFF;
  delay(1000);
  digitalWrite(13, LOW);
  myVariable = 0x55;
  delay(1000);
}