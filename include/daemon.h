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

extern void checkAlerts(struct CWInfo cwinfo, Metrics *metrics, Args *args, Config *config);

extern pid_t startDaemon(Args args, Config config, struct CWInfo cwinfo);

extern void writeHistoryS(System system, struct CWInfo cwinfo);
extern void writeHistoryM(Metrics metrics, struct CWInfo cwinfo);

extern void readDaemonS(struct CWInfo cwinfo, System *system);
extern void readDaemonM(struct CWInfo cwinfo, Metrics *metrics);
extern void readDaemonSM(struct CWInfo cwinfo, System *system, Metrics *metrics);

extern void readHistoryS(struct CWInfo cwinfo, char *path, System *system);
extern void readHistoryM(struct CWInfo cwinfo, char *path, Metrics *metric);

#endif