
void processLine(char *line);
void readSerial();
void cmd_mount(int argc, char *argv[]);
void cmd_DIR(int argc, char *argv[]);
void cmd_CD(int argc, char *argv[]);
void cmd_start(int argc, char *argv[]);
void cmd_stop(int argc, char *argv[]);
void cmd_cfg_set(int argc, char *argv[]);
void cmd_help(int, char**);

void cfg_set(int argc, char *argv[]);
void cmd_cfg_get(int argc, char *argv[]);
void motor_start(int argc, char *argv[]);
void motor_stop(int argc, char *argv[]); 
