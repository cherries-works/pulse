#ifndef DAEMON_H
#define DAEMON_H

#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#include "parse.h"
#include "utils.h"
#include "config.h"

struct shmbuf {
    pthread_mutex_t lock;
    Metrics metrics;
    System system;
};

extern void checkAlerts(Metrics metrics, Args args, Config *config);

extern pid_t startDaemon(Args args, Config config);

extern void writeHistoryS(System system);
extern void writeHistoryM(Metrics metrics);

extern void readDaemonS(System *system);
extern void readDaemonM(Metrics *metrics);
extern void readDaemonSM(System *system, Metrics *metrics);

extern void readHistoryS(char *path, System *system);
extern void readHistoryM(char *path, Metrics *metric);

#endif