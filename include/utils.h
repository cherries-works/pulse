#ifndef UTILS_H
#define UTILS_H

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
extern void convertTimeInSecondsToString(long unsigned seconds, char* buffer);

extern size_t readFile(const char *file_name, size_t size, char *buffer);
extern size_t sizeFile(const char *file_name);

#endif