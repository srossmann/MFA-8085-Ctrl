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

enum HEX_FILE_STATUS
{
  OK,
  END_OF_FILE,
  INVALID_START_CODE,
  INVALID_DATA_COUNT,
  EXCEED_16_BYTES_DATA,
  INVALID_ADDRESS,
  INVALID_RECORD_TYPE,
  UNSUPPORTED_RECORD_TYPE,
  INVALID_DATA,
  INVALID_CHECKSUM,
  INVALID_END_OF_FILE
};

enum SERIAL_STATE
{
  BEGIN,
  COMMAND,
  SPACE,
  PARAM,
  AVOID
};

 
void getParentPath(String &path);
bool chkFileExistence();
String getFirstDir(String &path);
String getActualPath(String &path);
void printDir(String &path);
void chgDir(String &path);
void dumpFile(String &path);
void loadFile(String &path);
bool chkPath(String &actualPath, String &path);
bool chkSDC();
bool chkFileExistence();
String getActualPath(String &path);
String getFirstDir(String &path);
void getParentPath(String &path);
HEX_FILE_STATUS hexFileToMem(File hexFile, uint16_t &lineCount);
HEX_FILE_STATUS getLine(File hexFile, uint8_t &dataLen, uint16_t &addr, uint8_t *dataBuf);
bool getByte(File hexFile, uint8_t &chr);
bool getHexCodedNibble(File hexFile, uint8_t &nibble);
bool getChar(File hexFile, uint8_t &chr);
void writeROM();
 
void releaseBusControl();
void writeMem(uint16_t addr, uint8_t dt);
uint8_t readMem(uint16_t addr);
void writeIO(uint16_t addr, uint8_t dt);
uint8_t readIO(uint16_t addr);
void Memorydump(int adresse);
void cmdAttache();



String activePath;
String chkFile = "";
bool isReadySD = false;

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
  // Serial.print(F("\nLoading ROM..."));
  // writeROM();
  // Serial.println(F("done"));

  //  Serial.println(F("\nTo begin, please send any word via Serial Monitor..."));
  cmdAttache();
}

void loop()
{
  static String command = "";
  static String param = "";
  static SERIAL_STATE serialStat = BEGIN;
  char inChar;
  File file;
  Serial.flush();
  Serial.print(">");
  while (true)
  {

    while (Serial.available() > 0)
    {

      inChar = Serial.read();
      Serial.print(inChar);
      if (inChar == '\n')
      {
        command.toUpperCase();
        param.toUpperCase();

        // Serial.println("command " + command);
        // Serial.println("param " + param);
        int i = 0;
        if (serialStat == COMMAND)
        {
          // Serial.println("COMMAND");
          i = command.length();
          command = command.substring(0, i - 1);
        }
        else if (serialStat == PARAM)
        {
          // Serial.println("PARAM");
          i = param.length();
          param = param.substring(0, i - 1);
        }

        

        if (command == "ATTACH")
        {
          cmdAttache();
        }

        if (command == "EJECT")
        {
          if (isReadySD)
          {
            SD.end();
            isReadySD = false;
            Serial.println(F("\nSD card ejected"));
          }
          else
            Serial.println(F("\nSD card already been ejected"));
        }

        if (command == "DIR")
          printDir(param);

        if (command == "CD")
          chgDir(param);

        if (command == "DUMP")
          dumpFile(param);

        if (command == "LOAD")
          loadFile(param);

        if (command == "READ")
        {
          File myFile;

          myFile = SD.open("test1.txt", FILE_WRITE);

          // if the file opened okay, write to it:
          if (myFile)
          {
            Serial.print("Writing to test1.txt...");
            myFile.println("testing 1, 2, 3.");
            // close the file:
            myFile.close();
            Serial.println("done.");
          }
          else
          {
            // if the file didn't open, print an error:
            Serial.println("error opening test1.txt");
          }

          // re-open the file for reading:
          myFile = SD.open("test1.txt");
          if (myFile)
          {
            Serial.println("read test1.txt:");

            // read from the file until there's nothing else in it:
            while (myFile.available())
            {
              Serial.write(myFile.read());
            }
            // close the file:
            myFile.close();
          }
          else
          {
            // if the file didn't open, print an error:
            Serial.println("error opening test.txt");
          }
        }

        if (command == "HELP")
        {
          Serial.println(F("*********************************"));
          Serial.println(F("***          HILFE            ***"));
          Serial.println(F("*** ATTACH = Start SD-Card    ***"));
          Serial.println(F("\"EJECT\""));
          Serial.println(F("   Must be called before ejecting current SD card."));
          Serial.println(F("\"DIR [directory_path]\""));
          Serial.println(F("   List all available folder or file in the specified directory_path."));
          Serial.println(F("\"CD [directory_path]\""));
          Serial.println(F("   Change current active directory to the specified directory_path."));
          Serial.println(F("\"DUMP file_path\""));
          Serial.println(F("   Printout all the content of the specified file_path."));
          Serial.println(F("\"LOAD file_path\""));
          Serial.println(F("   Load the specified file_path to the MPF-1."));
        }

        if (command == "TEST")
        {
          Serial.println("Exists: \"" + String(SD.exists(param)) + "\"");
        }

        if (command == "HOLD")
        {
          if (param == "ON")
          {
            Serial.println("BUSREQ ON");
            drive_BUSREQ();
          }

          if (param == "OFF")
          {
            Serial.println("BUSREQ OFF");
            release_BUSREQ();
          }
        }

        if (command == "MEM")
        {
          int adresse = 0;
          char s[4];
          param.toCharArray(s, 10, 0);
          adresse = (int)strtol(s, 0, 16);
          Memorydump(adresse);
        }

        if (command == "RESET")
        {
          drive_RST();
          delay(1000);
          release_RST();
        }

        if (command == "WRITEIO")
        {
          Serial.println("WRITEIO");
          writeIO(2, 22);
        }

        command = "";
        param = "";
        serialStat = BEGIN;
        Serial.print(">");
      }
      else if (inChar == ' ')
      {
        if (serialStat == COMMAND)
          serialStat = SPACE;
        else if (serialStat == PARAM)
          serialStat = AVOID;
      }
      else
      {
        switch (serialStat)
        {
        case BEGIN:
          command += inChar;
          serialStat = COMMAND;

          break;
        case COMMAND:
          command += inChar;

          break;
        case SPACE:
          param += inChar;
          serialStat = PARAM;
          break;
        case AVOID:
          break;
        case PARAM:
          param += inChar;
        }
      }
    }
  }
}

 

void cmdAttache()
{
  File file;
  if (isReadySD && chkFileExistence())
    Serial.println(F("\nSD card already been attached"));
  else
  {
    if (isReadySD)
    {
      SD.end();
      isReadySD = false;
    }
    Serial.print(F("\nIntializing SD card..."));
    if (SD.begin(SDCARD_CS_PIN))
    {
      Serial.println(F("done"));
      file = SD.open("").openNextFile();
      if (file)
      {
        chkFile = String(file.name());
        file.close();
      }
      else
      {
        chkFile = "";
        Serial.println(F("Warning: SD card is empty"));
      }
      isReadySD = true;
      activePath = "";
    }
    else
      Serial.println(F("failed"));
  }
}

void printMemory(int ad)
{
  // int ad = 0x8000;
  uint8_t dt;

  char logString[20];
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

void printDir(String &path)
{
  String actualPath;
  if (!chkPath(actualPath, path))
    return;
  File dir = SD.open(actualPath);
  if (!dir.isDirectory())
  {
    getParentPath(actualPath);
    Serial.println("\nDirectory of \"/" + actualPath + "\":");
    Serial.print(String(dir.name()) + '\t');
    if (String(dir.name()).length() < 8)
      Serial.print('\t');
    Serial.println(String(dir.size(), DEC));
  }
  else
  {
    File entry;
    Serial.println("\nDirectory of \"/" + actualPath + "\":");
    while (true)
    {
      entry = dir.openNextFile();
      if (!entry)
        break;
      Serial.print(String(entry.name()) + '\t');
      if (String(entry.name()).length() < 8)
        Serial.print('\t');
      Serial.println((entry.isDirectory()) ? "<DIR>" : String(entry.size(), DEC));
      entry.close();
    }
  }
  dir.close();
}

void chgDir(String &path)
{
  String actualPath;
  if (!chkPath(actualPath, path))
    return;
  File dir = SD.open(actualPath);
  if (!dir.isDirectory())
    Serial.print("\n\"/" + actualPath + "\" isn't a directory");
  else
    activePath = actualPath;
  dir.close();
  Serial.println("\nActive directory: \"/" + activePath + '\"');
}

void dumpFile(String &path)
{
  if (path == "")
  {
    Serial.println(F("\nNo file to dump"));
    return;
  }
  String actualPath;
  if (!chkPath(actualPath, path))
    return;
  File file = SD.open(actualPath);
  if (file.isDirectory())
    Serial.println("\n\"/" + actualPath + "\" isn't a file");
  else
  {
    Serial.println("\nContent of \"/" + actualPath + "\":");
    while (file.available())
      Serial.write(file.read());
    Serial.println();
  }
  file.close();
}

void loadFile(String &path)
{
  if (path == "")
  {
    Serial.println(F("\nNo file to load"));
    return;
  }
  String actualPath;
  if (!chkPath(actualPath, path))
    return;
  File file = SD.open(actualPath);
  if (file.isDirectory())
    Serial.println("\n\"/" + actualPath + "\" isn't a file");
  else
  {
    Serial.print("\nLoading \"" + actualPath + "\"...");
    uint16_t line;
    HEX_FILE_STATUS stat = hexFileToMem(file, line);
    if (stat == END_OF_FILE)
      Serial.println(F("done"));
    else
    {
      Serial.println("failed on line " + String(line));
      switch (stat)
      {
      case INVALID_START_CODE:
        Serial.println(F("Error: Invalid start code"));
        break;
      case INVALID_DATA_COUNT:
        Serial.println(F("Error: Invalid data count"));
        break;
      case EXCEED_16_BYTES_DATA:
        Serial.println(F("Error: Exceed 16-bytes data per line"));
        break;
      case INVALID_ADDRESS:
        Serial.println(F("Error: Invalid address value"));
        break;
      case INVALID_RECORD_TYPE:
        Serial.println(F("Error: Invalid record type"));
        break;
      case UNSUPPORTED_RECORD_TYPE:
        Serial.println(F("Error: Unsupported record type"));
        break;
      case INVALID_DATA:
        Serial.println(F("Error: Invalid data"));
        break;
      case INVALID_CHECKSUM:
        Serial.println(F("Error: Invalid checksum"));
        break;
      case END_OF_FILE:
        Serial.println(F("Done"));
        break;
      case OK:
        Serial.println(F("OK"));
        break;
      case INVALID_END_OF_FILE:
        Serial.println(F("Error: Invalid End Of File"));
      }
    }
  }
  file.close();
}

bool chkPath(String &actualPath, String &path)
{
  if (!chkSDC())
    return false;
  actualPath = getActualPath(path);
  if (actualPath != "" && !SD.exists(actualPath))
  {
    Serial.println("\nCan't find \"/" + actualPath + '\"');
    return false;
  }
  return true;
}

bool chkSDC()
{
  if (isReadySD)
  {
    if (chkFileExistence())
      return true;
    else
    {
      SD.end();
      isReadySD = false;
      Serial.println(F("\nCan't access SD card"));
      Serial.println(F("SD card ejected"));
    }
  }
  else
    Serial.println(F("\nSD card wasn't ready"));
  return false;
}

bool chkFileExistence()
{
  if (chkFile == "")
    return true;
  return SD.exists(chkFile);
}

String getActualPath(String &path)
{
  String actualPath = (path.startsWith("/")) ? "" : activePath;
  String dir;
  while (true)
  {
    dir = getFirstDir(path);
    if (dir == "" && path == "")
      break;
    else if (dir == "..")
      getParentPath(actualPath);
    else if (dir != "" && dir != ".")
    {
      if (actualPath == "")
        actualPath = dir;
      else
        actualPath += '/' + dir;
    }
  }
  return actualPath;
}

String getFirstDir(String &path)
{
  String dir;
  int idx = path.indexOf('/');
  if (idx >= 0)
  {
    dir = path.substring(0, idx);
    path.remove(0, idx + 1);
  }
  else
  {
    dir = path;
    path = "";
  }
  return dir;
}

void getParentPath(String &path)
{
  int idx = path.lastIndexOf('/');
  if (idx >= 0)
    path.remove(idx);
  else
    path = "";
}

HEX_FILE_STATUS hexFileToMem(File hexFile, uint16_t &lineCount)
{
  uint8_t dataLen, i;
  uint16_t addr;
  uint8_t dataBuf[16];
  HEX_FILE_STATUS stat;

  lineCount = 1;
  driveBusControl();
  while (true)
  {
    stat = getLine(hexFile, dataLen, addr, dataBuf);
    if (stat != OK)
      break;
    for (i = 0; i < dataLen; ++i)
      writeMem(addr++, dataBuf[i]);
    ++lineCount;
  }
  releaseBusControl();
  return stat;
}

HEX_FILE_STATUS getLine(File hexFile, uint8_t &dataLen, uint16_t &addr, uint8_t *dataBuf)
{
  uint8_t recType, chksum, chr;

  while (true)
  {
    if (!getChar(hexFile, chr))
      return INVALID_START_CODE;
    if (chr == ':')
      break;
    if (chr != '\r' && chr != '\n')
      return INVALID_START_CODE;
  }

  if (!getByte(hexFile, dataLen))
    return INVALID_DATA_COUNT;
  if (dataLen > 16)
    return EXCEED_16_BYTES_DATA;
  chksum = dataLen;

  if (!getByte(hexFile, chr))
    return INVALID_ADDRESS;
  addr = chr << 8;
  chksum += chr;
  if (!getByte(hexFile, chr))
    return INVALID_ADDRESS;
  addr |= chr;
  chksum += chr;

  if (!getByte(hexFile, recType))
    return INVALID_RECORD_TYPE;
  if (recType > 5)
    return INVALID_RECORD_TYPE;
  if (recType > 1)
    return UNSUPPORTED_RECORD_TYPE;
  chksum += recType;

  if (recType == 0)
  {
    for (uint8_t i = 0; i < dataLen; ++i)
    {
      if (!getByte(hexFile, chr))
        return INVALID_DATA;
      dataBuf[i] = chr;
      chksum += chr;
    }
  }

  if (!getByte(hexFile, chr))
    return INVALID_CHECKSUM;
  chksum = ~chksum;
  chksum += 1;
  if (chksum != chr)
    return INVALID_CHECKSUM;

  if (recType == 0)
    return OK;
  return (dataLen == 0) ? END_OF_FILE : INVALID_END_OF_FILE;
}

bool getByte(File hexFile, uint8_t &chr)
{
  uint8_t by, nibble;
  if (!getHexCodedNibble(hexFile, nibble))
    return false;
  by = nibble << 4;
  if (!getHexCodedNibble(hexFile, nibble))
    return false;
  chr = by | nibble;
  return true;
}

bool getHexCodedNibble(File hexFile, uint8_t &nibble)
{
  uint8_t chr;
  if (!getChar(hexFile, chr))
    return false;
  if (isDigit(chr))
    chr &= 15;
  else
  {
    chr &= 0xDF;
    chr -= 55;
  }
  if (chr > 15)
    return false;
  nibble = chr;
  return true;
}

bool getChar(File hexFile, uint8_t &chr)
{
  if (!hexFile.available())
    return false;
  chr = hexFile.read();
  return true;
}








