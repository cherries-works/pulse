#ifndef DAEMON_H
#define DAEMON_H

#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#include "parse.h"
#include "utils.h"
#include "config.h"

#define HISTORY_INTERVAL 3600

typedef struct {
    pthread_mutex_t lock;
    Metrics metrics;
    System system;
} CWData;

extern void checkAlerts(CWInfo cwinfo, Metrics *metrics, Args *args, Config *config);

extern pid_t startDaemon(Args args, Config config, CWInfo cwinfo);

extern void writeHistoryS(System system, CWInfo cwinfo);
extern void writeHistoryM(Metrics metrics, CWInfo cwinfo);

extern CWData *openSHM(CWInfo cwinfo);
extern void readDaemonS(CWInfo cwinfo, CWData *shmp, System *system);
extern void readDaemonM(CWInfo cwinfo, CWData *shmp, Metrics *metrics);
extern void readDaemonSM(CWInfo cwinfo, CWData *shmp, System *system, Metrics *metrics);

extern void readHistoryS(CWInfo cwinfo, char *path, System *system);
extern void readHistoryM(
    CWInfo cwinfo,
    int metric_index,
    char *entry_storer,
    size_t entry_storer_size
);

#endif