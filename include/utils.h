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
#include "setup.h"

extern const char *PROC_DIR;
extern const char *PROC_UPTIME_FILE;
extern const char *PROC_MEM_FILE;
extern const char *PROC_CPU_FILE;
extern const char *PROC_STAT_FILE;
extern const char *PROC_DISK_FILE;
extern const char *PROC_LOAD_FILE;
extern const char *PROC_NET_FILE;

extern const char *ETC_OS_RELEASE;
extern const char *OS_TYPE_SETTINGS[];

extern const char SPACE_IN_ASCII;
extern const unsigned ASCII_INT_TO_STRING_NUMNER;
extern const size_t BUFFER_ONE_KB;
extern const size_t BUFFER_ONE_MB;

extern void trim(char *buffer);
extern bool startsWith(char *src, size_t src_size, char *comp, size_t comp_size);
extern void escapeJson(const char *src, char *dst, size_t size);

extern void clearLine();
extern void clearLines(short i);

extern void formatTimeHumanReadable(uint64_t seconds, char* buffer, size_t size);
extern void formatTime(time_t _time, char *buffer, size_t size);
extern uint64_t unformatTime(char *buffer);

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
    WATCH,
    REMOTE,
} Command;

typedef struct {
    Command command;
    
    bool web; // for the MONITOR command
    bool headless; // for the MONITOR command
    
    bool json; // for the INFO / SNAPSHOT / TOP / PROCESS / PRUNE / CONFIG / VERSION command
    
    bool reset; // for the CONFIG command
    
    bool hash; // for the VERSION command
    
    bool current; // for the CONFIG command

    bool no_color; // for MONITOR / INFO / HELP / SNAPSHOT / TOP / PROCESS / WATCH command 

    Sort sort; // for the MONITOR / TOP command

    uint8_t style; // for the MONITOR command (1/2/3)

    uint16_t port; // for the MONITOR command
    unsigned sleep; // for the MONITOR command
    unsigned processes; // for the MONITOR / TOP command
    unsigned history_snapshot_interval; // for the MONITOR command
 
    pid_t pid; // for the PROCESS / WATCH command
    char name[256]; // for the PROCESS / WATCH command

    Prune prune; // for the PRUNE command
    unsigned keep; // for the PRUNE command
    char until[64]; // for the PRUNE command
} CWArgs;

extern CWArgs parseArgs(CWInfo cwinfo, int argc, char* argv[]);

extern void stop(CWInfo cwinfo);
extern void help(CWInfo cwinfo);
extern void top(CWArgs cwargs, CWInfo cwinfo);
extern void info(CWArgs cwargs, CWInfo cwinfo);
extern void monitor(CWArgs cwargs, CWConfig cwconfig, CWInfo cwinfo);
extern void process(CWArgs cwargs, CWInfo cwinfo);
extern void prune(CWArgs cwargs, CWInfo cwinfo);
extern void config(CWArgs cwargs, CWInfo cwinfo);
extern void snapshot(CWArgs cwargs, CWInfo cwinfo);
extern void version(CWArgs cwargs);
extern void handle(CWArgs cwargs, CWConfig cwconfig, CWInfo cwinfo);

#endif
