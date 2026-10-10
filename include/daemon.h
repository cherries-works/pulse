#ifndef DAEMON_H
#define DAEMON_H

#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#include "parse.h"
#include "utils.h"
#include "config.h"

struct CWData {
    pthread_mutex_t lock;
    Metrics metrics;
    System system;
};

extern void checkAlerts(CWInfo cwinfo, Metrics *metrics, Args *args, Config *config);

extern pid_t startDaemon(Args args, Config config, CWInfo cwinfo);

extern void writeHistoryS(System system, CWInfo cwinfo);
extern void writeHistoryM(Metrics metrics, CWInfo cwinfo);

extern void readDaemonS(CWInfo cwinfo, System *system);
extern void readDaemonM(CWInfo cwinfo, Metrics *metrics);
extern void readDaemonSM(CWInfo cwinfo, System *system, Metrics *metrics);

extern void readHistoryS(CWInfo cwinfo, char *path, System *system);
extern void readHistoryM(CWInfo cwinfo, char *path, Metrics *metric);

#endif