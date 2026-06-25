#ifndef DAEMON_H
#define DAEMON_H

#include <stdlib.h>
#include <semaphore.h>

#include "parse.h"
#include "utils.h"

struct shmbuf {
    sem_t  sem1;            /* POSIX unnamed semaphore */
    sem_t  sem2;            /* POSIX unnamed semaphore */
    size_t cnt;             /* Number of bytes used in 'buf' */

    Metrics metrics;
    System system;
    // char   buf[BUF_SIZE];   /* Data being transferred */
};


extern void startDaemon(PulseArgs args);
extern void readDaemonS(System *system);
extern void readDaemonM(Metrics *metrics);
extern void readDaemonSM(System *system, Metrics *metrics);

#endif