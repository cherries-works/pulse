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

void setupDaemon(pid_t pid) {
    char *home = getenv("HOME");
    if(home == NULL) {
        _log(L_ERROR, "No HOME environment variable");
        return;
    }

    char path_dir[BUFFER_ONE_KB / 2];
    snprintf(path_dir, BUFFER_ONE_KB / 2, "%s/%s/state", home, R_CHERRIES_FOLDER_PULSE);
    mkdir(path_dir, 0755);

    char daemon_path[BUFFER_ONE_KB];
    snprintf(daemon_path, BUFFER_ONE_KB, "%s/daemon.%d", path_dir, pid);

    FILE *f = fopen(daemon_path, "a");
    fclose(f);


    size_t time_buffer_size = BUFFER_ONE_KB / 8;
    char time_buffer[time_buffer_size];
    time_t log_time = getCurrentLog();
    formatTime(log_time, time_buffer, time_buffer_size);

    size_t history_path_size = BUFFER_ONE_KB;
    char history_path[history_path_size];
    
    snprintf(history_path, history_path_size, "%s/%s/history/%s", home, R_CHERRIES_FOLDER_PULSE, time_buffer);
    mkdir(history_path, 0755);
    
    snprintf(history_path, history_path_size, "%s/%s/history/%s/system", home, R_CHERRIES_FOLDER_PULSE, time_buffer);
    mkdir(history_path, 0755);

    snprintf(history_path, history_path_size, "%s/%s/history/%s/metric", home, R_CHERRIES_FOLDER_PULSE, time_buffer);
    mkdir(history_path, 0755);

    shm_unlink(CHERRIES_PULSE_SHM);
    int shm_fd = shm_open(CHERRIES_PULSE_SHM, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (shm_fd == -1) {
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(struct shmbuf)) == -1) {
        exit(EXIT_FAILURE);
    }

    struct shmbuf *shmp;
    shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shmp == MAP_FAILED) {
        exit(EXIT_FAILURE);
    }
}

// check if there is already daemon running
pid_t checkDaemon() {
    char *home = getenv("HOME");
    if(home == NULL) {
        _log(
            L_ERROR,
            "No HOME environment variable"
        );
        return -1;
    }

    char path_dir[BUFFER_ONE_KB / 2];
    snprintf(path_dir, BUFFER_ONE_KB / 2, "%s/%s/state", home, R_CHERRIES_FOLDER_PULSE);
    mkdir(path_dir, 0755);

    char file_path[BUFFER_ONE_KB];

    DIR *dir = opendir(path_dir);
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL) {
        char *name = entry->d_name;
        if(strcmp(name, ".") == 0) continue;
        if(strcmp(name, "..") == 0) continue;
        snprintf(file_path, BUFFER_ONE_KB, "%s/%s", path_dir, name);
        
        char *d = strchr(name, '.');
        if(d == NULL) continue;
        *d = '\0';

        if(strcmp(name, "daemon") != 0) continue;
        name = d + 1;
        int pid = atoi(name);

        closedir(dir);
        return pid;
    }

    closedir(dir);
    return -1;
}

pid_t startDaemon(Args args, Config config) {
    _log(
        L_INFO,
        "Starting Daemon"
    );

    pid_t pid = checkDaemon();
    if(pid > 0) {
        _log(L_INFO, "Attached to previous Daemon");
        sem_t *ready_sem = sem_open(CHERRIES_PULSE_READY_SEM, 0);
        if (ready_sem == SEM_FAILED) {
            _log(L_ERROR, "Failed to open ready semaphore");
            exit(EXIT_FAILURE);
        }
        sem_post(ready_sem);
        sem_close(ready_sem);
        
        return pid;
    }

    pid = fork();
    if (pid == 0) {
        _log(L_INFO, "Setting up Daemon.");
        setupDaemon(getpid());

        sem_t *ready_sem = sem_open(CHERRIES_PULSE_READY_SEM, 0);
        if (ready_sem == SEM_FAILED) {
            _log(L_ERROR, "Failed to open ready semaphore");
            exit(EXIT_FAILURE);
        }
        sem_post(ready_sem);
        sem_close(ready_sem);


        int shm_fd = shm_open(CHERRIES_PULSE_SHM, O_RDWR, 0);
        if(shm_fd == -1) {
            _log(
                L_ERROR,
                "SHM open failed"
            );
            exit(EXIT_FAILURE);
        }

        struct shmbuf *shmp;
        shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (shmp == MAP_FAILED) {
            _log(
                L_ERROR,
                "Mapping object failed"
            );
            exit(EXIT_FAILURE);
        }

        System system_snapshot = getSystem(args);
        System prev_system_snapshot = system_snapshot;

        Metrics metrics = getMetrics(prev_system_snapshot, system_snapshot);

        shmp->metrics = metrics;
        shmp->system = system_snapshot;

        memcpy(&shmp->metrics, &metrics, sizeof(Metrics));
        memcpy(&shmp->system, &system_snapshot, sizeof(System));

        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED); // cross-process mutex
        pthread_mutex_init(&shmp->lock, &attr);
        pthread_mutexattr_destroy(&attr);

        short repetition = 0;
        while(true) {
            if(repetition % 2 == 0) {
                writeHistoryM(metrics);
                writeHistoryS(system_snapshot);
                repetition = 0;
            }

            repetition++;

            prev_system_snapshot = system_snapshot;
            system_snapshot = getSystem(args);
            metrics = getMetrics(prev_system_snapshot, system_snapshot);

            pthread_mutex_lock(&shmp->lock);
            shmp->metrics = metrics;
            shmp->system = system_snapshot;
            pthread_mutex_unlock(&shmp->lock);

            checkAlerts(metrics, args, &config);
            sleep(args.sleep);
        }

        shm_unlink(CHERRIES_PULSE_SHM);
        exit(EXIT_SUCCESS);
    }

    return pid;
}
