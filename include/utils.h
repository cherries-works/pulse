#ifndef UTILS_H
#define UTILS_H

extern char *PROC_DIR;
extern char *PROC_UPTIME_FILE;
extern char *PROC_MEM_FILE;
extern char *PROC_CPU_FILE;
extern char *PROC_STAT_FILE;
extern char *PROC_DISK_FILE;
extern char *PROC_LOAD_FILE;
extern char *PROC_NET_FILE;

extern char SPACE_IN_ASCII;
extern unsigned ASCII_INT_TO_STRING_NUMNER;
extern unsigned BUFFER_ONE_KB;
extern unsigned BUFFER_ONE_MB;

extern void trim(char *buffer);
extern void clearLine();
extern void clearLines(unsigned i);
extern void convertTimeInSecondsToString(unsigned seconds, char* buffer);

extern int readFile(char *file_name, size_t size, char *buffer);
extern long sizeFile(char *file_name);

#endif