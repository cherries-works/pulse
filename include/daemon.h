#ifndef DAEMON_H
#define DAEMON_H

#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#include "parse.h"
#include "utils.h"
#include "config.h"

// Counts per second.
//
// HISTORY_INTERVAL is 60 * 60 for one hour
// ARCHIVE_INTERVAL is 3600 * 24 for one day
// DELETION_INTERVAL is 24 hours * 30 for fixed thirty-days.
//
#define HISTORY_INTERVAL 60 * 60
#define ARCHIVE_INTERVAL (HISTORY_INTERVAL * 24)
#define DELETION_INTERVAL (ARCHIVE_INTERVAL * 30)

typedef struct {
    pthread_mutex_t lock;
    Metrics metrics;
    System system;
} CWData;

extern void checkAlerts(CWInfo cwinfo, Metrics *metrics, Args *args, Config *config);

extern pid_t startDaemon(Args args, Config config, CWInfo cwinfo);

extern void writeHistoryS(System system, CWInfo cwinfo);
extern void writeHistoryM(Metrics metrics, CWInfo cwinfo);

extern void archiveHistory(CWInfo cwinfo);
extern void deleteHistoryArchives(CWInfo cwinfo);

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