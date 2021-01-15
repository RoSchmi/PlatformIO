// Some simple samples for my own use, showing allocating and check for memory leaks

// Some important data about the ram usage can be seen in this directory on your PC:
// user\.platformio\packages\framework-arduino-samd-seeed\variants\wio_terminal\linker_scripts\gcc

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */

#include <Arduino.h>
#include <rpcWiFi.h>
#include "TFT_eSPI.h"
#include "Free_Fonts.h" //include the header file
TFT_eSPI tft;
int current_text_line = 0;

#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define LCD_FONT FreeSans9pt7b
#define LCD_LINE_HEIGHT 18



// Routine to send messages to the display
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
  
  Serial.begin(9600);
  Serial.println("\r\nStarting");
  lcd_log_line((char *)"Starting");

  //******************************************************************************
  /*
  // This code snippet can be used to get the actual address on the heap
  // and to preset a memory segment with pattern 0xAA55AA55
  uint32_t * ptr_one;
  uint32_t * last_ptr_one;
    
  
   for (volatile int i = 0; 1 < 1000; i++)
   {
     last_ptr_one = ptr_one;
     ptr_one = 0;
     ptr_one = (uint32_t *)malloc(1);
     if (ptr_one == 0)
     {
       ptr_one = last_ptr_one;
       volatile int dummy68424 = 1;
     }
     else
     {
       *ptr_one = (uint32_t)0xAA55AA55;
       char printBuf[25];
       
       if (((uint32_t)ptr_one % 256)== 0)
       {
         sprintf(printBuf, "%09x", (uint32_t)ptr_one);
          lcd_log_line((char*)printBuf);
          volatile int dummy68524 = 1;
       }
     } 
   }
   volatile int dummy485 = 1;
   */

  //******************************************************************************
   
   
  // Fills memory from 0x20028F80 - 0x2002FE00 with pattern AA55
  // So you can see at breakpoints how much of heap was used
  /*
  uint32_t * ptr_two;

  ptr_two = (uint32_t *)0x20028F80;
  while (ptr_two < (uint32_t *)0x2002fe00)
  {
    *ptr_two = (uint32_t)0xAA55AA55;
     ptr_two++;
  }
  */

 //******************************************************************************

 // This code snippet allocates 20 bytes on the heap and copies a string to this address

 //uint32_t * ptr_three;
 char * ptrChar = 0;

 ptrChar = (char *)pvPortMalloc(20);
  //vPortFree(ptr_one);

  char theStr[] = "RoSchmi\0";

  strcpy(ptrChar, theStr);

//******************************************************************************

}

uint32_t * ptr_three = 0;
uint32_t * last_ptr_three = 0;
uint32_t loopCtr = 0;
uint32_t lostLeakageBytes = 0;
uint32_t referenceHeapAddr = 0;
char buf[30] {0};

// For freeRTos see:
  //https://www.freertos.org/FreeRTOS_Support_Forum_Archive/November_2013/freertos_Direct_using_pvPortMalloc_and_vPortFree_032bc6bfj.html

void loop() {
 
  // This code snippet shows allocating and memory leak
  // Clear screen
  if (current_text_line == 12)
  {
    current_text_line = 0;
    tft.fillScreen(TFT_WHITE);
  }
  
   ptr_three = (uint32_t *)malloc(0x20);
   sprintf(buf, "2) Allocated at: %10X", (unsigned int)ptr_three);
   lcd_log_line(buf);
   char charArr3[100]{0};
   strcpy(charArr3, "Roland");
   strcpy((char*)ptr_three, (char *)charArr3); 
   ptr_three = (uint32_t *)malloc(0x20);
   sprintf(buf, "2) Allocated at: %10X", (unsigned int)ptr_three);
   lcd_log_line(buf);


  if (loopCtr == 0)
  {
    referenceHeapAddr = (uint32_t)ptr_three;
  }

  lostLeakageBytes =  (uint32_t)ptr_three - referenceHeapAddr;
  loopCtr++;
  sprintf(buf, " Lost %i",  lostLeakageBytes);
  lcd_log_line(buf);
  Serial.println(buf);
  

  volatile int dummy5851 = 1;
  delay(1000);
}