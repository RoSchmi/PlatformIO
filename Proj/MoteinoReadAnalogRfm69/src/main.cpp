// Copyright RoSchmi 2020 License Apache V 2.0
// This App for Moteino (or Arduino pro mini) reads analog signals from pins A1, 
// A2 and A3 in intervals as determined in const "readIntervalLengthSec"
// The library "TimeOut" is used to produce callbacks every second
// In the callback the counter "readIntervalCounter" is incremented
// In the Loop() method "readIntervalCounter" is checked and if "readIntervalLengthSec"
// is reached, new analog values are read, processed, and (optionally) displayed on OLED and console
// and sent via Rfm96. Then "readIntervalCounter" is resetted.

// To calculate for example temperature values from the analog readings
// enter two X/Y Pairs to define the linear functions
// e.g. in "CollectorSensorParams", "StorageSensorParams" and "WaterSensorParams"
// the calculation is done in function "countsToValueFloat()
// To test enter X-Values in the console and get corresponding Y-Values

// Hint: Code example for uses with MCP3428
//https://github.com/ControlEverythingCommunity/MCP3428


#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "TimeOut.h"

// To include libraries, event. this must be added to platformio.ini;
// lib_deps = TimeOut

//https://github.com/NitrofMtl/TimeOut


//#define SERIAL_EN                //comment out if you don't want any serial output
#define OLED_EN                  //comment out if you don't want any OLED output

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
//#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

/*
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
*/

#define LED           9 // Moteinos have LEDs on D9

#ifdef SERIAL_EN
  //#define SERIAL_BAUD   115200
  #define SERIAL_BAUD   9600
  #define DEBUG(input)   {Serial.print(input);}
  #define DEBUGln(input) {Serial.println(input);}
  #define SERIALFLUSH() {Serial.flush();}
#else
  #define DEBUG(input);
  #define DEBUGln(input);
  #define SERIALFLUSH();
#endif

typedef struct curveValuePairs {
  int Xmin_Counts;
  int Xmax_Counts;
  float Ymin_Value;
  float Ymax_Value;
};

// For each Sensor enter the correspondig values of two valuepairs X1/Y1
curveValuePairs CollectorSensorParams = {
                                    .Xmin_Counts = 16,
                                    .Xmax_Counts = 108,
                                    .Ymin_Value = 1.340,
                                    .Ymax_Value = 1.487
  };

  curveValuePairs StorageSensorParams = {
                                    .Xmin_Counts = 39,
                                    .Xmax_Counts = 73,
                                    .Ymin_Value = 1.252,
                                    .Ymax_Value = 1.610
  };

 curveValuePairs WaterSensorParams = {
                                    .Xmin_Counts = 52,
                                    .Xmax_Counts = 56,
                                    .Ymin_Value = 1.393,
                                    .Ymax_Value = 1.410
  };

const int readIntervalLengthSec = 5;
const int readIntervalMaxLengthSec = 3600; // not longer than 1 hour, should only be changed if needed
int readIntervalCounter = 0;
bool readIvcAccessIsAllowed = true;

Interval interval0; //start instance, instance have to be into global scope

int CollectorAnalogPin = A1;
int StorageAnalogPin = A2;
int WaterAnalogPin = A3;

int valCollector = 0;  // variable to store the value read 
int valStorage = 0;
int valWater = 0; 

String readResult = "";

const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data

boolean newData = false;


// Forward declarations:
void intervalPrint0();
void showNewData();
void recvWithEndMarker();
void testdrawchar(void);
float countsToValueFloat(int, curveValuePairs);

/*
uint8_t initialize(uint8_t freqBand, uint8_t nodeID, uint8_t networkID);
void setHighPower(bool onOff);
void setPowerLevel(uint8_t powerLevel);
void encrypt(const char* key);
bool sendMessage(int pActState, int pOldState, uint16_t pRepeatSend, uint32_t pTimeFromLast_MS = 0, uint32_t pCurrent = 0, uint32_t pPower = 0, uint32_t pWork= 0);
void setMode(uint8_t newMode);
bool receiveDone();
*/

void setup() {
  // put your setup code here, to run once:
#ifdef SERIAL_EN
  Serial.begin(SERIAL_BAUD);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  #endif

  pinMode(LED, OUTPUT);
#ifdef SERIAL_EN
  Serial.println("\r\nEnter new x-Value");
#endif

 // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();
  delay(500);
  
 interval0.interval(1000, intervalPrint0); //delay,callback function (calling interval could be inside any function) 

//  interval3.interval<char*, int, String, float>(5000, callbackWithArguments, "this argument timer call after; ", 5000, " micro second ", 5.5 );
  
//  class1.setInterval(4000);
#ifdef SERIAL_EN
  Interval::printContainer(Serial);
#endif
}

void loop() {
  Interval::handler(); //main monitor handler, Have to be into void loop
  if (readIntervalCounter > readIntervalLengthSec)
  {
        readIvcAccessIsAllowed = false;
        digitalWrite(LED, HIGH);
        delay(500);
        digitalWrite(LED, LOW);

    	  valCollector = analogRead(CollectorAnalogPin);  // read the input pin
        valStorage = analogRead(StorageAnalogPin);
        valWater = analogRead(WaterAnalogPin);  

      #ifdef SERIAL_EN      
        Serial.print("Read CollectorValue = ");
        Serial.println(valCollector);
           
        Serial.print("Read StorageValue = ");
        Serial.println(valStorage);
       
        Serial.print("Read WaterValue = ");
        Serial.println(valWater);
        Serial.println("");
      #endif

      #ifdef OLED_EN
        display.clearDisplay();
        display.display();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.print("Coll: ");
        display.println(valCollector);
        //display.println(countsToValueFloat(valCollector, CollectorSensorParams), 2);       
        display.setCursor(0,10);
        display.print("Stor: ");
        display.println(valStorage);
        display.setCursor(0,20);
        display.print("Water: ");
        display.println(valWater);
        display.display();
    #endif
 
      readIntervalCounter = 0;
      readIvcAccessIsAllowed = true;
    }

    recvWithEndMarker();
    if (newData == true)
    {
    readResult =  receivedChars;
    
    Serial.println(countsToValueFloat(readResult.toInt(), CollectorSensorParams), 3);
    
    }   
    showNewData();
}

float countsToValueFloat(int X_in, curveValuePairs cVP)
{
    float slope = (cVP.Ymax_Value - cVP.Ymin_Value) / (cVP.Xmax_Counts - cVP.Xmin_Counts);
    float XinMalSlope = X_in * slope;
    float Ydisplacement =  cVP.Ymin_Value - cVP.Xmin_Counts * slope;
    
    Serial.print("Slope = ");
    Serial.println(slope,3);
    Serial.print("X_in mal slope = ");
    Serial.println(XinMalSlope, 3);
    Serial.print("Displacement = ");
    Serial.println(Ydisplacement, 3);
    Serial.print("Function is: Y = ");
    Serial.print(slope, 3);
    Serial.print(" * X + ");
    Serial.println(Ydisplacement, 3);
    Serial.print("Y = ");
       
    return XinMalSlope + Ydisplacement;
  }

void recvWithEndMarker() 
{
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;
 
  // if (Serial.available() > 0) {
  while (Serial.available() > 0 && newData == false) 
  {
    rc = Serial.read();

    if (rc != endMarker) 
    {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) 
      {
        ndx = numChars - 1;
      }
    }
    else 
    {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void showNewData() {
 if (newData == true) {
 Serial.print("You entered: ");
 Serial.println(receivedChars);
 Serial.println("\r\nEnter new x-Value");
 newData = false;
 }
}

void intervalPrint0() {
  // Serial.println("Interval 0 have been trigged");
  if (readIvcAccessIsAllowed && (readIntervalCounter < readIntervalMaxLengthSec))
  {
    readIntervalCounter++;
    // Serial.println("Interval Counter incremented");
  }
  
}

/*
void callbackWithArguments(char* msg1, int value1, String msg2, float value2) {
  Serial.print(msg1);
  Serial.print(value1);
  Serial.print(msg2);
  Serial.println(value2);
}
*/

