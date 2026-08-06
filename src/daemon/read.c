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

static struct shmbuf *openSHM() {
    int fd = shm_open(CHERRIES_PULSE_SHM, O_RDWR, 0);
    if (fd == -1) {
        _log(L_ERROR, "SHM open failed");
        exit(EXIT_FAILURE);
    }

    struct shmbuf *shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        _log(L_ERROR, "Mapping object failed");
        exit(EXIT_FAILURE);
    }

    return shmp;
}

void readDaemonSM(System *system, Metrics *metrics) {
    struct shmbuf *shmp = openSHM();

    pthread_mutex_lock(&shmp->lock);
    memcpy(metrics, &shmp->metrics, sizeof(Metrics));
    memcpy(system, &shmp->system, sizeof(System));
    pthread_mutex_unlock(&shmp->lock);
}

void readDaemonS(System *system) {
    struct shmbuf *shmp = openSHM();

    pthread_mutex_lock(&shmp->lock);
    memcpy(system, &shmp->system, sizeof(System));
    pthread_mutex_unlock(&shmp->lock);
}

void readDaemonM(Metrics *metrics) {
    struct shmbuf *shmp = openSHM();

    pthread_mutex_lock(&shmp->lock);
    memcpy(metrics, &shmp->metrics, sizeof(Metrics));
    pthread_mutex_unlock(&shmp->lock);
}

void readHistoryS(
    char *path,
    System *system
) {
    size_t buffer_size = BUFFER_ONE_KB * 4;
    char buffer[buffer_size];

    FILE *f = fopen(path, "r");
    if(f == NULL) {
        _log(L_ERROR, "History path invalid.");
        return;
    }

    size_t size = fread(buffer, 1, buffer_size - 1, f);
    if(size == 0) {
        _log(L_ERROR, "Reading history file failed.");
        return;
    }

    buffer[size] = '\0';

    char *buffer_pointer = buffer;
    char *n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';
    
    system->cpu.idle = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';
    
    system->cpu.processes = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';

    system->cpu.total = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';
    
    system->disk.available = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';

    system->disk.read = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';
    
    system->disk.total = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';

    system->disk.write = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';
    
    system->load.load1 = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';

    system->load.load5 = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';
    
    system->load.load15 = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';

    system->memory.available = strtoul(buffer_pointer, NULL, 10);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';

    system->memory.total = strtoul(buffer_pointer, NULL, 10);

    fclose(f);
}

void readHistoryM(
    char *path,
    Metrics *metric
) {
    size_t buffer_size = BUFFER_ONE_KB * 4;
    char buffer[buffer_size];

    FILE *f = fopen(path, "r");
    if(f == NULL) {
        _log(L_ERROR, "History path invalid.");
        return;
    }

    size_t size = fread(buffer, 1, buffer_size - 1, f);
    if(size == 0) {
        _log(L_ERROR, "Reading history file failed.");
        return;
    }

    buffer[size] = '\0';

    char *buffer_pointer = buffer;
    char *n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';
    
    metric->cpuUsage = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';
    
    metric->diskUsage = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';
    
    metric->ramUsage = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';
    
    metric->read = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';
    
    metric->write = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';
    
    metric->rx = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    n = strchr(buffer_pointer, '\n');
    if(n == NULL) {
        fclose(f);
        return;
    }
    *n = '\0';
    
    metric->tx = (float)atof(buffer_pointer);
    buffer_pointer = n + 1;

    fclose(f);
}
