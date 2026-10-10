#ifndef LOG_H
#define LOG_H

#include "setup.h"

typedef enum {
    L_INFO,
    L_ERROR
} log_types;

extern int setupLog();
extern void _log(
    struct CWInfo cwinfo,
    log_types type, 
    const char *message
);
extern int printLogFile(struct CWInfo cwinfo);

#endif