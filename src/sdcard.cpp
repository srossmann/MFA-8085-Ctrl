#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "sdcard.h"

String activePath;
String chkFile = "";
bool isReadySD = false;

 
uint8_t intelHexChecksum(uint8_t *data, uint8_t len) {
  uint16_t sum = 0;
  for (uint8_t i = 0; i < len; i++) {
    sum += data[i];
  }
  return (uint8_t)(~sum + 1);
}

bool chkFileExistence()
{
  if (chkFile == "")
    return true;
  return SD.exists(chkFile);
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

void cmdUnmount()
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

void cmdMount()
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

void mkDir(String &path)
{
   
  
  bool mk = SD.mkdir(path);
  if (mk)
  {
    chgDir(path);
  }
  
  
   
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

 