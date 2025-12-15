#include "avr/io.h"
#include "8085Bus.h"
#include <Arduino.h>
#include "rom.h"


void printMemory(int ad)
{
  // int ad = 0x8000;
  uint8_t dt;

  char logString[20];
  Serial.println("------------------------------------------------------------------------------------------");
  for (int i1 = ad; i1 < ad + 255; i1 = i1 + 16)
  {
    sprintf(logString, "%04X", i1);
    Serial.print(logString);
    Serial.print(" : ");

    for (int i2 = 0; i2 < 16; i2++)
    {

      dt = readMem(i1 + i2);
      sprintf(logString, "%02X ", dt);
      Serial.print(logString);
    }

    Serial.print(" - ");

    for (int i2 = 0; i2 < 16; i2++)
    {
      dt = readMem(i1 + i2);

      if (dt > 31 and dt < 127)
      {
        char someChar = (char)dt;
        Serial.print(someChar);
      }
      else
      {
        Serial.print(".");
      }

      Serial.print(" ");
    }
    Serial.println("");
    /* code */
  }
  Serial.println("------------------------------------------------------------------------------------------");
}

void Memorydump(int adresse)
{
  int ad = adresse;
  driveBusControl();
  printMemory(ad);

  Serial.println("W=Weiter X=Ende");
  Serial.flush();
  bool b1 = true;
  while (b1)
  {

    while (Serial.available() > 0)
    {

      char inChar;
      inChar = Serial.read();

      if (inChar == 'X' or inChar == 'x')
      {
        Serial.flush();
        b1 = false;
        break;
      }
      if (inChar == 'W' or inChar == 'w')
      {
        ad = ad + 256;
        printMemory(ad);
        Serial.println("W=Weiter X=Ende");
        Serial.flush();
      }
      Serial.flush();
    }
  }
  releaseBusControl();
}

void writeROM()
{
  driveBusControl();
  for (uint16_t i = 0; i < ROM_SIZE; ++i)
    writeMem(ROM_ADDRESS + i, pgm_read_byte(ROM_DATA + i));
  releaseBusControl();
}