#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "utils.h"
#include "daemon.h"
#include "log.h"

static struct CWData *openSHM(struct CWInfo cwinfo) {
    int fd = shm_open(CHERRIES_PULSE_SHM, O_RDWR, 0);
    if (fd == -1) {
        perror("shm_open");
        fprintf(stderr, "Shared memory name: %s\n", CHERRIES_PULSE_SHM);
        return NULL;
    }

    struct CWData *shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        return MAP_FAILED;
    }

    return shmp;
}

void readDaemonSM(struct CWInfo cwinfo, System *system, Metrics *metrics) {
    struct CWData *shmp = openSHM(cwinfo);
    if (shmp == NULL) {
        _log(
            cwinfo,
            L_ERROR,
            "Failed to open SHM."
        );
        return;
    }
    if (shmp == MAP_FAILED) {
        _log(
            cwinfo,
            L_ERROR,
            "MMAP failed on SHM."
        );
        return;
    }

    int rc = pthread_mutex_lock(&shmp->lock);
    if (rc != 0) {
        _log(
            cwinfo,
            L_ERROR,
            "pthread mutex lock failed!"
        );
        return;
    }

    memcpy(metrics, &shmp->metrics, sizeof(*metrics));
    memcpy(system, &shmp->system, sizeof(*system));

    rc = pthread_mutex_unlock(&shmp->lock);
    if (rc != 0) {
        _log(
            cwinfo,
            L_ERROR,
            "pthread mutex unlock failed!"
        );
    }
}

void readDaemonS(struct CWInfo cwinfo, System *system) {
    struct CWData *shmp = openSHM(cwinfo);
    if (shmp == NULL || shmp == MAP_FAILED) {
        _log(
            cwinfo,
            L_ERROR,
            "Failed to open SHM."
        );
        return;
    }

    int rc = pthread_mutex_lock(&shmp->lock);
    if (rc != 0) {
        _log(
            cwinfo,
            L_ERROR,
            "pthread mutex lock failed!"
        );
        return;
    }

    memcpy(system, &shmp->system, sizeof(*system));

    rc = pthread_mutex_unlock(&shmp->lock);
    if (rc != 0) {
        _log(
            cwinfo,
            L_ERROR,
            "pthread mutex unlock failed!"
        );
    }
}


void readDaemonM(struct CWInfo cwinfo, Metrics *metrics) {
    struct CWData *shmp = openSHM(cwinfo);
    if (shmp == NULL || shmp == MAP_FAILED) {
        _log(
            cwinfo,
            L_ERROR,
            "Failed to open SHM."
        );
        return;
    }

    int rc = pthread_mutex_lock(&shmp->lock);
    if (rc != 0) {
        _log(
            cwinfo,
            L_ERROR,
            "pthread mutex lock failed!"
        );
        return;
    }

    memcpy(metrics, &shmp->metrics, sizeof(*metrics));

    rc = pthread_mutex_unlock(&shmp->lock);
    if (rc != 0) {
        _log(
            cwinfo,
            L_ERROR,
            "pthread mutex unlock failed!"
        );
    }
}

void readHistoryS(
    struct CWInfo cwinfo,
    char *path,
    System *system
) {
    size_t buffer_size = BUFFER_ONE_KB * 4;
    char buffer[buffer_size];

    FILE *f = fopen(path, "r");
    if(f == NULL) {
        _log(cwinfo, L_ERROR, "History path invalid [readHistoryS].");
        return;
    }

    size_t size = fread(buffer, 1, buffer_size - 1, f);
    if(size == 0) {
        _log(cwinfo, L_ERROR, "Reading history file failed [S].");
        return;
    }

    buffer[size] = '\0';

    char *buffer_pointer = buffer;
    char *n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [S].");
        fclose(f);
        return;
    }
    *n = '\0';
    
    system->cpu.idle = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [S].");
        fclose(f);
        return;
    }
    *n = '\0';
    
    system->cpu.processes = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [S].");
        fclose(f);
        return;
    }
    *n = '\0';

    system->cpu.total = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [S].");
        fclose(f);
        return;
    }
    *n = '\0';
    
    system->disk.available = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [S].");
        fclose(f);
        return;
    }
    *n = '\0';

    system->disk.read = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [S].");
        fclose(f);
        return;
    }
    *n = '\0';
    
    system->disk.total = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [S].");
        fclose(f);
        return;
    }
    *n = '\0';

    system->disk.write = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [S].");
        fclose(f);
        return;
    }
    *n = '\0';
    
    system->load.load1 = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [S].");
        fclose(f);
        return;
    }
    *n = '\0';

    system->load.load5 = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [S].");
        fclose(f);
        return;
    }
    *n = '\0';
    
    system->load.load15 = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [S].");
        fclose(f);
        return;
    }
    *n = '\0';

    system->memory.available = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [S].");
        fclose(f);
        return;
    }
    *n = '\0';

    system->memory.total = strtoul(buffer_pointer, NULL, 10);
    fclose(f);
}

void readHistoryM(
    struct CWInfo cwinfo,
    char *path,
    Metrics *metric
) {
    size_t buffer_size = BUFFER_ONE_KB * 4;
    char buffer[buffer_size];

    FILE *f = fopen(path, "r");
    if(f == NULL) {
        _log(cwinfo, L_ERROR, "History path invalid [readHistoryM].");
        return;
    }

    size_t size = fread(buffer, 1, buffer_size - 1, f);
    if(size == 0) {
        _log(cwinfo, L_ERROR, "Reading history file failed [M].");
        return;
    }

    buffer[size] = '\0';

    char *buffer_pointer = buffer;
    char *n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [M].");
        fclose(f);
        return;
    }
    *n = '\0';
    
    metric->cpuUsage = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [M].");
        fclose(f);
        return;
    }
    *n = '\0';
    
    metric->diskUsage = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [M].");
        fclose(f);
        return;
    }
    *n = '\0';
    
    metric->ramUsage = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [M].");
        fclose(f);
        return;
    }
    *n = '\0';
    
    metric->read = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [M].");
        fclose(f);
        return;
    }
    *n = '\0';
    
    metric->write = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [M].");
        fclose(f);
        return;
    }
    *n = '\0';
    
    metric->rx = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        _log(cwinfo, L_ERROR, "Corrupt History [M].");
        fclose(f);
        return;
    }
    *n = '\0';
    
    metric->tx = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    fclose(f);
}
