#include <Arduino.h>
#define SDCARD_CS_PIN 53

 
void cmdMount();
void cmdUnmount();

bool chkFileExistence();
bool chkPath(String &actualPath, String &path);
bool chkSDC();
String getActualPath(String &path);
String getFirstDir(String &path);
void getParentPath(String &path);
void printDir(String &path);
void chgDir(String &path);
void dumpFile(String &path);
void mkDir(String &path);
void dumpFile(String &path);



