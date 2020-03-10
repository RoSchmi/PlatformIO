#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
pinMode(13,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
delay(500);              // wait for a second
digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
delay(500);              // wait for a second

}

