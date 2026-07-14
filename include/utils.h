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

extern const char SPACE_IN_ASCII;
extern const unsigned ASCII_INT_TO_STRING_NUMNER;
extern const size_t BUFFER_ONE_KB;
extern const size_t BUFFER_ONE_MB;

extern void trim(char *buffer);
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

typedef struct {
    volatile bool running;
    
    bool web;
    bool help;
    bool headless;

    bool stop; // stop the current running processes
    
    Sort sort;
    uint16_t port;
    unsigned sleep;
    unsigned processes;
} Args;

extern Args parseArgs(int argc, char* argv[]);

#endif