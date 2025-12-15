
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
void cmd_MEM(int argc, char *argv[]) {
   
  if (argc < 2) {
    Serial.println("MEM <adresse"); 
    return;
  }

  int value = atoi(argv[1]);
  Memorydump(value);
}
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
    Serial.print(" ");
    Serial.println(table[i].pname);

    if (table[i].sub != NULL) {
      printHelp(table[i].sub, table[i].subCount, indent + 1);
    }
  }
}

/* ---------------- Sub Command     ---------------- */
const command_t cfgCommands[] = {
  { "SET","\t\t <wert> ", cmd_cfg_set, NULL, 0 },
  { "GET", "\t\t <wert> rückgabe ",cmd_cfg_get, NULL, 0 },
};

const command_t cfgSubCommands[] = {
  { "PARM1","\t\t <wert> ", cmd_cfg_set, NULL, 0 },
  { "PARM2", "\t\t <wert> rückgabe ",cmd_cfg_get, NULL, 0 },
};

const command_t sdcardCommands[] = {
  { "OPEN","\t\t open ", cmd_mount, NULL, 0 },
  { "CLOSE", "\t\t close ", cmd_unmount,  NULL, 0 },
};

/* ---------------- Root Command   ---------------- */
const command_t rootCommands[] = {
  { "HELP", " \t\t Gibt eine Hilfe-Liste aus ", cmd_help, NULL, 0 },
  { "SD", "\t\t Speicherkarte ", NULL, sdcardCommands, 2 },
  { "DIR", "\t\t File liste  ", cmd_DIR, NULL, 0 },
  { "CD", " \t\t Path wechseln ", cmd_CD, NULL, 0 },
  { "MKDIR", "\t\t Dir erstellen ", cmd_MKDIR, NULL, 0 },
  { "DUMP", "\t\t File anzeigen ", cmd_DUMP, NULL, 0 },
  { "MEM", "\t\t File anzeigen ", cmd_MEM, NULL, 0 },




  { "CFG", "\t\t Konfiguration ",  NULL, cfgCommands,   2 },
};




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
        Serial.print(">");
        return;
      }
    }
  }

  Serial.print("Unbekannter Befehl: ");
  Serial.println(argv[level]);
  Serial.print(">");
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
    }
    else if (c != '\r' && cmdIndex < CMD_BUFFER_SIZE - 1) {
      cmdBuffer[cmdIndex++] = c;
      Serial.print(c);
    }
  }
}


 