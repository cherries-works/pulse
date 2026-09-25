#ifndef UTILS_H
#define UTILS_H

#ifndef PULSE_VERSION
#define PULSE_VERSION "unknown"
#endif

#ifndef PULSE_COMMIT
#define PULSE_COMMIT "unknown"
#endif

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>

#include "config.h"

extern const char *PROC_DIR;
extern const char *PROC_UPTIME_FILE;
extern const char *PROC_MEM_FILE;
extern const char *PROC_CPU_FILE;
extern const char *PROC_STAT_FILE;
extern const char *PROC_DISK_FILE;
extern const char *PROC_LOAD_FILE;
extern const char *PROC_NET_FILE;

extern const char *ETC_OS_RELEASE;

extern const char SPACE_IN_ASCII;
extern const unsigned ASCII_INT_TO_STRING_NUMNER;
extern const size_t BUFFER_ONE_KB;
extern const size_t BUFFER_ONE_MB;

extern void trim(char *buffer);
extern bool startsWith(char *src, size_t src_size, char *comp, size_t comp_size);

extern void clearLine();
extern void clearLines(short i);

extern void formatTimeHumanReadable(long unsigned seconds, char* buffer, size_t size);
extern void formatTime(time_t _time, char *buffer, size_t size);
extern long unsigned unformatTime(char *buffer);

extern size_t readFile(const char *file_name, size_t size, char *buffer);
extern size_t sizeFile(const char *file_name);

extern void cleanDir(char *path);
extern unsigned countDir(char *path);

extern const char* CHERRIES_FOLDER;
extern const char* CHERRIES_FOLDER_PULSE;
extern const char* R_CHERRIES_FOLDER;
extern const char* R_CHERRIES_FOLDER_PULSE;

extern const char* SYS_THERMAL_ZONE;

extern const char* CHERRIES_PULSE_SHM;
extern const char* CHERRIES_PULSE_READY_SEM;

extern const char* CHERRIES_DEFAULT_TOML;

typedef enum {
    CPU,
    RAM
} Sort;

typedef enum {
    LOGS,
    HISTORY,
    ALL
} Prune;

typedef enum {
    MONITOR,
    TOP,
    INFO,
    STOP,
    HELP,
    PROCESS,
    PRUNE,
    CONFIG,
    SNAPSHOT,
    VERSION,
} Command;

typedef struct {
    Command command;
    
    bool web; // for the MONITOR command
    bool headless; // for the MONITOR command
    
    bool json; // for the SNAPSHOT command
    
    bool reset; // for the CONFIG command
    
    bool hash; // for the VERSION command

    Sort sort; // for the MONITOR / TOP command

    uint16_t port; // for the MONITOR command
    unsigned sleep; // for the MONITOR command
    unsigned processes; // for the MONITOR / TOP command
 
    pid_t process; // for the PROCESS command

    Prune prune; // for the PRUNE command
    unsigned keep; // for the PRUNE command
    char until[64]; // for the PRUNE command
} Args;

extern Args parseArgs(int argc, char* argv[]);

extern void stop();
extern void help();
extern void top(Args args);
extern void info(Args args);
extern void monitor(Args args, Config config);
extern void process(Args args);
extern void prune(Args args);
extern void config(Args args);
extern void snapshot(Args args);
extern void version(Args args);

#endif