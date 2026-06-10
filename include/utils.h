extern void trim(char *buffer);
extern void clearLine();
extern void clearLines(unsigned i);
extern int readFile(char *file_name, size_t size, char *buffer);
extern void convertTimeInSecondsToString(unsigned seconds, char* buffer);