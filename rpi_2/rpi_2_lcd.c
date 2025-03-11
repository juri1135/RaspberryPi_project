#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>
#include "rpi_2.h"
#include <string.h>
#define Detected_DEVICE_ID_BY_I2C       0x27        // Device ID detected by I2C 
                                                    // Seems that it maps to the device's address
#define LCD_BACKLIGHT_ON                0x08        // On             "0000 1000"
#define LCD_BACKLIGHT_OFF               0x00        // Off            "0000 0000"
#define LCD_ENABLE                      0x04        // Enable         "0000 0100"
#define LCD_DISABLE                     0x00        // Disable        "0000 0000"
#define LCD_RW_READ                     0x02        // Read           "0000 0010" 
#define LCD_RW_WRITE                    0x00        // Write          "0000 0000"
#define LCD_RS_DATA                     0x01        // Data           "0000 0001"
#define LCD_RS_INST                     0x00        // Instruction    "0000 0000"


int deviceHandle;                                   // Seems that it maps to the register address of the device

void toggleLCDEnable(int eightBits)
{
    delayMicroseconds(500);
    if (wiringPiI2CWrite(deviceHandle, (eightBits | LCD_ENABLE)) < 0) {
        perror("[ERROR] wiringPiI2CWrite (Enable HIGH) failed");
    }
    delayMicroseconds(500);

    if (wiringPiI2CWrite(deviceHandle, (eightBits | LCD_DISABLE)) < 0) {
        perror("[ERROR] wiringPiI2CWrite (Enable LOW) failed");
    }
    delayMicroseconds(500);
}

void sendBitsToLCD(int eightBits, int mode)
{
    int highFourBits = mode | (eightBits & 0xF0) | LCD_BACKLIGHT_ON;
    int lowFourBits = mode | ((eightBits << 4) & 0xF0) | LCD_BACKLIGHT_ON;

    if (wiringPiI2CWrite(deviceHandle, highFourBits) < 0) {
        perror("[ERROR] wiringPiI2CWrite (highFourBits) failed");
    }
    toggleLCDEnable(highFourBits);

    if (wiringPiI2CWrite(deviceHandle, lowFourBits) < 0) {
        perror("[ERROR] wiringPiI2CWrite (lowFourBits) failed");
    }
    toggleLCDEnable(lowFourBits);
}

void initializeLCD()
{ 
    deviceHandle = wiringPiI2CSetup(Detected_DEVICE_ID_BY_I2C);  
  sendBitsToLCD(0x33, LCD_RS_INST | LCD_RW_WRITE);       // "0011 0011" (8-bit mode, 1 line, 5x8 dots)
  sendBitsToLCD(0x32, LCD_RS_INST | LCD_RW_WRITE);       // "0011 0010" (8-bit mode, 1 line, 5x8 dots)
  sendBitsToLCD(0x06, LCD_RS_INST | LCD_RW_WRITE);       // "0000 0110" (cursor move direction is increment, normal shift operation)
  sendBitsToLCD(0x0C, LCD_RS_INST | LCD_RW_WRITE);       // "0000 1100" (display on, cursor Off, blink off)
  sendBitsToLCD(0x28, LCD_RS_INST | LCD_RW_WRITE);       // "0010 1000" (4-bit mode, 2 lines, 5x8 dots)
  sendBitsToLCD(0x01, LCD_RS_INST | LCD_RW_WRITE);       // "0000 0001" (clear display)
  sendBitsToLCD(0x80, LCD_RS_INST | LCD_RW_WRITE);       
  delayMicroseconds(500);
}

// Display text string 
int displayText(int lineNum, char* text)
{   
    //lcd 화면 초기화
    sendBitsToLCD(0x01, LCD_RS_INST | LCD_RW_WRITE); 
  int len=strlen(text);
  if(lineNum==1)  sendBitsToLCD(0x80, LCD_RS_INST | LCD_RW_WRITE); 
  else sendBitsToLCD(0xC0, LCD_RS_INST | LCD_RW_WRITE); 
    while (*text) {
      if (*text == '\n') {
      text++;
      continue;
      }
      sendBitsToLCD(*text, LCD_RS_DATA);
      text++;
    }

    return len;
}


