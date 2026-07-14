#ifndef DAEMON_H
#define DAEMON_H

#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#include "parse.h"
#include "utils.h"

struct shmbuf {
    pthread_mutex_t lock;
    Metrics metrics;
    System system;
};

extern pid_t startDaemon(Args args);

extern void readDaemonS(System *system);
extern void readDaemonM(Metrics *metrics);
extern void readDaemonSM(System *system, Metrics *metrics);

extern void readHistoryS(char *path, System *system);
extern void readHistoryM(char *path, Metrics *metric);

#endif