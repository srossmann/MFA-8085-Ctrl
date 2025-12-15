/*
 * Z80_ROM_Emulator_and_RAM_Writer.ino
 *
 * Z80 pins --- MEGA2560 pins
 *       A0 --- PA0/AD0  (22)
 *       A1 --- PA1/AD1  (23)
 *       A2 --- PA2/AD2  (24)
 *       A3 --- PA3/AD3  (25)
 *       A4 --- PA4/AD4  (26)
 *       A5 --- PA5/AD5  (27)
 *       A6 --- PA6/AD6  (28)
 *       A7 --- PA7/AD7  (29)
 *       A8 --- PC0/A8   (37)
 *       A9 --- PC1/A9   (36)
 *      A10 --- PC2/A10  (35)
 *      A11 --- PC3/A11  (34)
 *      A12 --- PC4/A12  (33)
 *      A13 --- PC5/A13  (32)
 *      A14 --- PC6/A14  (31)
 *      A15 --- PC7/A15  (30)
 *
 *       D0 --- PL0      (49)
 *       D1 --- PL1      (48)
 *       D2 --- PL2      (47)
 *       D3 --- PL3      (46)
 *       D4 --- PL4      (45)
 *       D5 --- PL5      (44)
 *       D6 --- PL6      (43)
 *       D7 --- PL7      (42)
 *
 *      !WR --- PG0/!WR  (41)
 *      !RD --- PG1/!RD  (40)
 *    !MREQ --- PG2/ALE  (39)
 *   !IOREQ --- PD7/T0   (38)
 *  !ROM_WR --- PB7      (13)
 *   !RESET --- PB6      (12)
 *  !BUSACK --- PB5      (11)
 *  !BUSREQ --- PB4      (10)
 *
 *
 * Created 31 May 2020 (amid CoViD-19 outbreaks)
 * by ZulNs
 * @Gorontalo, Indonesia
 *
 * This code is in the public domain.
 *
 * https://github.com/ZulNs/Arduino_ROM_Emulator
 */

#ifndef USE_ROM_WR
#define USE_ROM_WR 1
#endif

#include <SPI.h>
#include <SD.h>
#include "8085Bus.h"
#include "cli.h"
#include "sdcard.h"


void setup()
{
  Serial.begin(19200);
  while (!Serial)
    ; // wait for serial port to connect. Needed for native USB port only
  Serial.println("");
  Serial.println("");
  Serial.println(F("*********************************"));
  Serial.println(F("*** MFA-Computer 8085 Control ***"));
  Serial.println(F("*** by Sigmar Roßmann         ***"));
  Serial.println(F("***                           ***"));
  Serial.println(F("*** 12.11.2025                ***"));
  Serial.println(F("*********************************"));
  Serial.println("");
  
  cmdMount();
  Serial.print(">");
}

void loop()
{
  
   readSerial();

  
}

 












