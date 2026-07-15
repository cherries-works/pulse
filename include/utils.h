#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

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

extern size_t readFile(const char *file_name, size_t size, char *buffer);
extern size_t sizeFile(const char *file_name);

extern const char* CHERRIES_FOLDER;
extern const char* CHERRIES_FOLDER_PULSE;
extern const char* R_CHERRIES_FOLDER;
extern const char* R_CHERRIES_FOLDER_PULSE;

extern const char* SYS_THERMAL_ZONE;

extern const char* CHERRIES_PULSE_SHM;
extern const char* CHERRIES_PULSE_READY_SEM;

typedef enum {
    CPU,
    RAM
} Sort;

typedef enum {
    MONITOR,
    TOP,
    INFO,
    STOP,
    HELP
} Command;

typedef struct {
    volatile bool running;
    
    bool web;
    bool headless;

    Sort sort;
    Command command;

    uint16_t port;
    unsigned sleep;
    unsigned processes;
} Args;

extern Args parseArgs(int argc, char* argv[]);

extern void stop();
extern void help();
extern void top(Args args);
extern void info(Args args);
extern void monitor(Args args);

extern pid_t getRenderPid();

#endif