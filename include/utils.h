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

extern const char *PROC_DIR;
extern const char *PROC_UPTIME_FILE;
extern const char *PROC_MEM_FILE;
extern const char *PROC_CPU_FILE;
extern const char *PROC_STAT_FILE;
extern const char *PROC_DISK_FILE;
extern const char *PROC_LOAD_FILE;
extern const char *PROC_NET_FILE;
extern const char *BAD_REQUEST;

extern const char *ETC_OS_RELEASE;
extern const char *OS_TYPE_SETTINGS[];

extern const char SPACE_IN_ASCII;
extern const uint8_t ASCII_INT_TO_STRING_NUMNER;
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

#define clear() printf("\033[H\033[J")
extern void cleanDir(char *path);
extern uint16_t countDir(char *path);

extern const char* CHERRIES_FOLDER;
extern const char* CHERRIES_FOLDER_PULSE;
extern const char* R_CHERRIES_FOLDER;
extern const char* R_CHERRIES_FOLDER_PULSE;

extern const char* SYS_THERMAL_ZONE;

extern const char* CHERRIES_PULSE_SHM;
extern const char* CHERRIES_PULSE_READY_SEM;

extern const char* CHERRIES_DEFAULT_TOML;

#endif
