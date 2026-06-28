#ifndef DAEMON_H
#define DAEMON_H

#include <stdlib.h>
#include <semaphore.h>

#include "parse.h"
#include "utils.h"

struct shmbuf {
    sem_t  sem1;
    sem_t  sem2;

    Metrics metrics;
    System system;
};


extern void startDaemon(PulseArgs args);
extern void readDaemonS(System *system);
extern void readDaemonM(Metrics *metrics);
extern void readDaemonSM(System *system, Metrics *metrics);

#endif