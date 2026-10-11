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
#include <inttypes.h>

#include "utils.h"
#include "daemon.h"
#include "log.h"

CWData *openSHM(CWInfo cwinfo) {
    int fd = shm_open(CHERRIES_PULSE_SHM, O_RDWR, 0);
    if (fd == -1) {
        return NULL;
    }

    CWData *cwdata = mmap(NULL, sizeof(*cwdata), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (cwdata == MAP_FAILED) {
        return MAP_FAILED;
    }

    return cwdata;
}

void readDaemonSM(CWInfo cwinfo, CWData *cwdata, System *system, Metrics *metrics) {
    int rc = pthread_mutex_lock(&cwdata->lock);
    if (rc != 0) {
        _log(
            cwinfo,
            L_ERROR,
            "pthread mutex lock failed!"
        );
        return;
    }

    memcpy(metrics, &cwdata->metrics, sizeof(*metrics));
    memcpy(system, &cwdata->system, sizeof(*system));

    rc = pthread_mutex_unlock(&cwdata->lock);
    if (rc != 0) {
        _log(
            cwinfo,
            L_ERROR,
            "pthread mutex unlock failed!"
        );
    }
}

void readDaemonS(CWInfo cwinfo, CWData *cwdata, System *system) {
    int rc = pthread_mutex_lock(&cwdata->lock);
    if (rc != 0) {
        _log(
            cwinfo,
            L_ERROR,
            "pthread mutex lock failed!"
        );
        return;
    }

    memcpy(system, &cwdata->system, sizeof(*system));

    rc = pthread_mutex_unlock(&cwdata->lock);
    if (rc != 0) {
        _log(
            cwinfo,
            L_ERROR,
            "pthread mutex unlock failed!"
        );
    }
}


void readDaemonM(CWInfo cwinfo, CWData *cwdata, Metrics *metrics) {
    int rc = pthread_mutex_lock(&cwdata->lock);
    if (rc != 0) {
        _log(
            cwinfo,
            L_ERROR,
            "pthread mutex lock failed!"
        );
        return;
    }

    memcpy(metrics, &cwdata->metrics, sizeof(*metrics));

    rc = pthread_mutex_unlock(&cwdata->lock);
    if (rc != 0) {
        _log(
            cwinfo,
            L_ERROR,
            "pthread mutex unlock failed!"
        );
    }
}

void appendLatestHistoryPoint(
    char *buffer,
    size_t buffer_size,
    uint64_t timestamp,
    float usage
) {
    size_t point_size = BUFFER_ONE_KB;
    char point[point_size];

    int _snprintf = snprintf(
        point,
        point_size,
        "{\"timestamp\":%" PRIu64 ",\"usage\":%f}",
        timestamp,
        usage
    );

    if (
        _snprintf < 0 || 
        (size_t)_snprintf >= point_size ||
        (size_t)_snprintf >= buffer_size
    ) {
        return;
    }

    size_t point_length = (size_t)_snprintf;
    size_t length = strlen(buffer);

    while (length + point_length + (length > 0 ? 1 : 0) + 1 > buffer_size) {
        char *comma = strchr(buffer, ',');

        if (comma != NULL) {
            memmove(buffer, comma + 1, strlen(comma + 1) + 1);
        } else {
            buffer[0] = '\0';
        }

        length = strlen(buffer);
    }

    if (length > 0) {
        buffer[length++] = ',';
        buffer[length] = '\0';
    }

    memcpy(buffer + length, point, point_length + 1);
}

void readHistoryM(
    CWInfo cwinfo,
    int metric_index,
    char *entry_storer,
    size_t entry_storer_size
) {
    size_t time_buffer_size = BUFFER_ONE_KB;
    char time_buffer[time_buffer_size];
    formatTime(cwinfo.started_at, time_buffer, time_buffer_size);

    time_t now = time(NULL);
    time_t first_interval = (time_t)cwinfo.started_at;
    first_interval -= first_interval % HISTORY_INTERVAL;

    time_t last_interval = now - (now % HISTORY_INTERVAL);

    size_t path_size = BUFFER_ONE_KB;
    char path[path_size];

    for (time_t interval = first_interval; interval <= last_interval; interval += HISTORY_INTERVAL) {
        int _snprintf = snprintf(
            path,
            path_size,
            "%s/history/%s/metric/%" PRIu64,
            cwinfo.pulse,
            time_buffer,
            (uint64_t)interval
        );

        if (_snprintf < 0 || (size_t)_snprintf >= path_size) {
            _log(cwinfo, L_ERROR, "History path too long [readHistoryM].");
            return;
        }

        FILE *f = fopen(path, "r");
        if (f == NULL) {
            continue;
        }

        uint64_t timestamp;
        float values[7];

        while (fscanf(f, "%" SCNu64, &timestamp) == 1) {
            bool complete = true;

            for (size_t i = 0; i < 7; i++) {
                if (fscanf(f, "%f", &values[i]) != 1) {
                    complete = false;
                    break;
                }
            }

            if (!complete) {
                break;
            }

            appendLatestHistoryPoint(
                entry_storer,
                entry_storer_size,
                timestamp,
                values[metric_index]
            );
        }

        fclose(f);
    }
}


void readHistoryS(
    CWInfo cwinfo,
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
        fclose(f);
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
