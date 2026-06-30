#ifndef DAEMON_H
#define DAEMON_H

#include <stdlib.h>
#include <semaphore.h>

#include "parse.h"
#include "utils.h"

struct shmbuf {
    pthread_mutex_t lock;
    Metrics metrics;
    System system;
};

extern pid_t startDaemon(PulseArgs args);

extern void readDaemonS(System *system);
extern void readDaemonM(Metrics *metrics);
extern void readDaemonSM(System *system, Metrics *metrics);

#endif