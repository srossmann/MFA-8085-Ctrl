
#include <Arduino.h>
#include "cli.h"
#include "sdcard.h"
#include "cpu.h"

#define CMD_BUFFER_SIZE 64
#define MAX_ARGS 4

char cmdBuffer[CMD_BUFFER_SIZE];
uint8_t cmdIndex = 0;

/* ---------------- Command Table ---------------- */

typedef void (*cmd_func_t)(int argc, char *argv[]);

typedef struct command_t {
  const char *name;
  const char *pname;
  cmd_func_t func;               // NULL wenn Sub-Commands existieren
  const struct command_t *sub;   // NULL wenn kein Sub-Command
  uint8_t subCount;
} command_t;


long parseNumber(const char* s) {
  // erkennt: 0xFF, FF, 123
  if (strstr(s, "0x") == s || strstr(s, "0X") == s)
    return strtol(s, NULL, 16);

  // enthält nur Hex-Zeichen?
  for (uint8_t i = 0; s[i]; i++) {
    if (!isxdigit(s[i])) return atol(s);
  }
  return strtol(s, NULL, 16);
}

/* ---------------- Command Handler ---------------- */

/******************************************/
/*  CMD MOUNT                             */
/******************************************/
void cmd_mount(int argc, char *argv[]) {
  Serial.println("Open SD");
  cmdMount();
}
/******************************************/
/*  CMD UNMOUNT                           */
/******************************************/
void cmd_unmount(int argc, char *argv[]) {
  Serial.println("Close SD");
  cmdUnmount();
}

/******************************************/
/*  CMD UNMOUNT                           */
/******************************************/
void cmd_cfg_get(int argc, char *argv[]) {
  Serial.println("CFG GET");
}

/******************************************/
/*  CMD UNMOUNT                           */
/******************************************/
void cmd_cfg_set(int argc, char *argv[]) {
  if (argc < 2) {
    Serial.println("SET <wert>");
    return;
  }

  int value = atoi(argv[1]);
  Serial.print("SET auf ");
  Serial.println(value);
}

/******************************************/
/*  CMD UNMOUNT                           */
/******************************************/
void cmd_DIR(int argc, char *argv[]) {
  String path = "/";
  String value = getActualPath(path);
  if (argc < 2) {
    printDir(value);
    return;
  }

  value = argv[1];
  printDir(value);
}

/******************************************/
/*  CMD UNMOUNT                           */
/******************************************/
void cmd_CD(int argc, char *argv[]) {
  String value = "/";
  if (argc < 2) {
    chgDir(value);
    return;
  }

  value = argv[1];
  chgDir(value);
}

/******************************************/
/*  CMD UNMOUNT                           */
/******************************************/
void cmd_MKDIR(int argc, char *argv[]) {
  String value = "";
  if (argc < 2) {
    Serial.println("MKDIR <path>"); 
    return;
  }

  value = argv[1];
  mkDir(value);
}

/******************************************/
/*  CMD UNMOUNT                           */
/******************************************/
void cmd_DUMP(int argc, char *argv[]) {
  String value = "";
  if (argc < 2) {
    Serial.println("DUMP <file>"); 
    return;
  }

  value = argv[1];
  dumpFile(value);
}

/******************************************/
/*  CMD UNMOUNT                           */
/******************************************/
void cmd_RESET(int argc, char *argv[]) {
   
  cpuReset();
}

/******************************************/
/*  CMD UNMOUNT                           */
/******************************************/
void cmd_MEM_DUMP(int argc, char *argv[]) {
   
  if (argc < 2) {
    Serial.println("MEM <adresse"); 
    return;
  }

  int adresse = 0;
  adresse = parseNumber(argv[2]);

  Memorydump(adresse);
}

/******************************************/
/*  CMD UNMOUNT                           */
/******************************************/
void cmd_MEM_WRITE(int argc, char *argv[]) {
   
  if (argc < 3) {
    Serial.println("MEM <adresse"); 
    return;
  }

  long adresse = parseNumber(argv[2]);
  long wert = parseNumber(argv[3]);

  writeMemory(adresse, wert);

  for (long i = adresse; i < (adresse + 255); i++)
  {
    writeMemory(i, wert);
  }
  

}

/******************************************/
/*  CMD UNMOUNT                           */
/******************************************/
void cmd_HALT(int argc, char *argv[]) {
   cpuHalt();
  
}

/******************************************/
/*  CMD UNMOUNT                           */
/******************************************/
void cmd_RUN(int argc, char *argv[]) {
   cpuRun();
   
}

/******************************************/
/*  CMD UNMOUNT                           */
/******************************************/
void cmd_IO_GET(int argc, char *argv[]) {
  if (argc < 3) {
    Serial.println("MEM <adresse"); 
    return;
  }
   
}

/******************************************/
/*  CMD UNMOUNT                           */
/******************************************/
void cmd_IO_SET(int argc, char *argv[]) {
   if (argc < 3) {
    Serial.println("MEM <adresse"); 
    return;
  }
   
}

/* ---------------- Sub Command     ---------------- */
const command_t cfgCommands[] = {
  { "SET","<wert> ", cmd_cfg_set, NULL, 0 },
  { "GET", "<wert> rückgabe ",cmd_cfg_get, NULL, 0 },
};

const command_t cfgSubCommands[] = {
  { "PARM1","<wert> ", cmd_cfg_set, NULL, 0 },
  { "PARM2", "<wert> rückgabe ",cmd_cfg_get, NULL, 0 },
};

const command_t memSubCommands[] = {
  { "DUMP","<adresse> ", cmd_MEM_DUMP, NULL, 0 },
  { "WRITE", "<adresse> <wert> rückgabe ",cmd_MEM_WRITE, NULL, 0 },
};

const command_t sdcardCommands[] = {
  { "OPEN","open ", cmd_mount, NULL, 0 },
  { "CLOSE", "close ", cmd_unmount,  NULL, 0 },
};

const command_t cpuSubCommands[] = {
  { "RESET","Reset 8085 ", cmd_RESET, NULL, 0 },
  { "HALT","Halt 8085 ", cmd_HALT, NULL, 0 }, 
  { "RUN","Run 8085 ", cmd_RUN, NULL, 0 }, 
};

const command_t ioSubCommands[] = {
  { "GET","Reset 8085 ", cmd_IO_GET, NULL, 0 },
  { "SET","Halt 8085 ", cmd_IO_SET, NULL, 0 }, 
   
};
/* ---------------- Root Command   ---------------- */
const command_t rootCommands[] = {
  { "HELP", "Gibt eine Hilfe-Liste aus ", cmd_help, NULL, 0 },
  { "SD", "Speicherkarte ", NULL, sdcardCommands, 2 },
  { "DIR", "File liste  ", cmd_DIR, NULL, 0 },
  { "CD", "Path wechseln ", cmd_CD, NULL, 0 },
  { "MKDIR", "Dir erstellen ", cmd_MKDIR, NULL, 0 },
  { "DUMP", "File anzeigen ", cmd_DUMP, NULL, 0 },
  { "MEM", "File anzeigen ", NULL, memSubCommands, 2 },
  { "IO", "File anzeigen ", NULL, ioSubCommands, 2 },
  { "CPU", "8085 ", NULL, cpuSubCommands, 3 },




  { "CFG", "Konfiguration ",  NULL, cfgCommands,   2 },
};



/******************************************/
/*  CMD UNMOUNT                           */
/******************************************/
void printHelp(
  const command_t *table,
  uint8_t tableSize,
  uint8_t indent
) {
  
 
  for (uint8_t i = 0; i < tableSize; i++) {
    for (uint8_t j = 0; j < indent; j++) {
      Serial.print("  ");
    }
    Serial.print(table[i].name);

    Serial.print("\t\t");
    Serial.println(table[i].pname);

    if (table[i].sub != NULL) {
      printHelp(table[i].sub, table[i].subCount, indent + 1);
    }
  }
}

void cmd_help(int argc, char *argv[]) {

  const uint8_t commandCount =
  sizeof(rootCommands) / sizeof(rootCommands[0]);

  // Nur HELP → alles anzeigen
  if (argc == 1) {
    Serial.println("Verfuegbare Befehle:");
    Serial.println("--------------------------------------------");
    printHelp(rootCommands, commandCount, 0);
    Serial.println("--------------------------------------------");
    return;
  }

  // HELP <command> → Sub-Commands anzeigen
  const command_t *table = rootCommands;
  uint8_t tableSize = 3;
  uint8_t level = 1;

  while (level < argc) {
    bool found = false;

    for (uint8_t i = 0; i < tableSize; i++) {
      if (strcmp(argv[level], table[i].name) == 0) {

        if (table[i].sub != NULL) {
          table = table[i].sub;
          
          tableSize = table[i].subCount;
          found = true;
          break;
        } else {
          Serial.println("Keine Sub-Commands vorhanden");
          return;
        }
      }
    }

    if (!found) {
      Serial.print("Unbekannter Befehl: ");
      Serial.println(argv[level]);
      return;
    }

    level++;
  }

  printHelp(table, tableSize, 0);
}


/* ---------------- Dispatcher ---------------- */
void dispatch(
  const command_t *table,
  uint8_t tableSize,
  int argc,
  char *argv[],
  uint8_t level
) {
  if (level >= argc) return;
   

  for (uint8_t i = 0; i < tableSize; i++) {
     
    if (strcmp(argv[level], table[i].name) == 0) {

      // Sub-Commands vorhanden → tiefer gehen
      if (table[i].sub != NULL) {
        dispatch(
          table[i].sub,
          table[i].subCount,
          argc,
          argv,
          level + 1
        );
        return;
      }

      // End-Command → Handler ausführen
      if (table[i].func != NULL) {
        table[i].func(argc, argv);
        //Serial.print(">");
        return;
      }
    }
  }

  Serial.print("Unbekannter Befehl: ");
  Serial.println(argv[level]);
 // Serial.print(">");
}


/* ---------------- Serial Reader ---------------- */

void processLine(char *line) {
  const uint8_t commandCount =
  sizeof(rootCommands) / sizeof(rootCommands[0]);
  char *argv[MAX_ARGS];
  int argc = 0;
  strupr(line);
  char *tok = strtok(line, " ");
  while (tok && argc < MAX_ARGS) {
    argv[argc++] = tok;
    tok = strtok(NULL, " ");
  }

  if (argc == 0) return;

  dispatch(rootCommands, commandCount, argc, argv, 0);
}

void readSerial() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      cmdBuffer[cmdIndex] = '\0';
      Serial.println("");
      processLine(cmdBuffer);
      cmdIndex = 0;
      Serial.print(">");
    }
    else if (c != '\r' && cmdIndex < CMD_BUFFER_SIZE - 1) {
      cmdBuffer[cmdIndex++] = c;
      Serial.print(c);
    }
  }
}


 