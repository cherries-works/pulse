#ifndef LOG_H
#define LOG_H

typedef enum {
    INFO,
    ERROR
} log_types;

extern void setupLog();
extern void _log(
    log_types types, 
    const char *message
);
extern void failureLog();
extern time_t getCurrentLog();

#endif