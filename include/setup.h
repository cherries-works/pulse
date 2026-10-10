#ifndef SETUP_H
#define SETUP_H

#include <signal.h>

typedef struct {
    time_t started_at;
    char home[256];
    char pulse[256];
    char log[256];
    char history[256];
} CWInfo;

extern CWInfo setup();

#endif