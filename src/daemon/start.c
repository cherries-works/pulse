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
        _log(ERROR, "No HOME environment variable");
        return;
    }

    char path_dir[512];
    sprintf(path_dir, "%s/%s/state", home, R_CHERRIES_FOLDER_PULSE);
    mkdir(path_dir, 0755);

    char daemon_path[1028];
    sprintf(daemon_path, "%s/daemon.%d", path_dir, pid);

    FILE *f = fopen(daemon_path, "a");
    fclose(f);

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

static struct shmbuf *openSHM() {
    int fd = shm_open(CHERRIES_PULSE_SHM, O_RDWR, 0);
    if (fd == -1) {
        _log(ERROR, "SHM open failed");
        exit(EXIT_FAILURE);
    }

    struct shmbuf *shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        _log(ERROR, "Mapping object failed");
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

void writeHistoryS(System system) {
    char *home = getenv("HOME");
    if(home == NULL) {
        _log(
            ERROR,
            "No HOME environment variable"
        );
        return;
    }

    time_t _time = time(NULL);

    char history_path[1028];
    sprintf(history_path, "%s/%s/history/system.%ld", home, R_CHERRIES_FOLDER_PULSE, _time);

    FILE *f = fopen(history_path, "a");
    char buffer[256];

    sprintf(buffer, "%ld\n", system.cpu.idle);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%ld\n", system.cpu.processes);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%ld\n", system.cpu.total);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%lld\n", system.disk.available);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%lld\n", system.disk.read);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%lld\n", system.disk.total);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%lld\n", system.disk.write);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%f\n", system.load.load1);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%f\n", system.load.load5);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%f\n", system.load.load15);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%ld\n", system.memory.available);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%ld\n", system.network.rx);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%ld\n", system.network.tx);
    fwrite(buffer, strlen(buffer), 1, f);


    sprintf(buffer, "%s\n", system.processes[0].name);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%ld\n", system.processes[0].cpu);
    fwrite(buffer, strlen(buffer), 1, f);
    
    sprintf(buffer, "%d\n", system.processes[0].pid);
    fwrite(buffer, strlen(buffer), 1, f);
    
    sprintf(buffer, "%ld\n", system.processes[0].ram);
    fwrite(buffer, strlen(buffer), 1, f);


    sprintf(buffer, "%s\n", system.processes[1].name);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%ld\n", system.processes[1].cpu);
    fwrite(buffer, strlen(buffer), 1, f);
    
    sprintf(buffer, "%d\n", system.processes[1].pid);
    fwrite(buffer, strlen(buffer), 1, f);
    
    sprintf(buffer, "%ld\n", system.processes[1].ram);
    fwrite(buffer, strlen(buffer), 1, f);
    
    
    sprintf(buffer, "%s\n", system.processes[2].name);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%ld\n", system.processes[2].cpu);
    fwrite(buffer, strlen(buffer), 1, f);
    
    sprintf(buffer, "%d\n", system.processes[2].pid);
    fwrite(buffer, strlen(buffer), 1, f);
    
    sprintf(buffer, "%ld\n", system.processes[2].ram);
    fwrite(buffer, strlen(buffer), 1, f);
    

    sprintf(buffer, "%ld", system.uptime);
    fwrite(buffer, strlen(buffer), 1, f);

    fclose(f);
}

void writeHistoryM(Metrics metrics) {
    char *home = getenv("HOME");
    if(home == NULL) {
        _log(
            ERROR,
            "No HOME environment variable"
        );
        return;
    }

    time_t _time = time(NULL);

    char history_path[1028];
    sprintf(history_path, "%s/%s/history/metrics.%ld", home, R_CHERRIES_FOLDER_PULSE, _time);

    FILE *f = fopen(history_path, "a");
    char buffer[256];

    sprintf(buffer, "%f\n", metrics.cpuUsage);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%f\n", metrics.diskUsage);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%f\n", metrics.ramUsage);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%f\n", metrics.read);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%f\n", metrics.write);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%f\n", metrics.rx);
    fwrite(buffer, strlen(buffer), 1, f);

    sprintf(buffer, "%f", metrics.tx);
    fwrite(buffer, strlen(buffer), 1, f);

    fclose(f);
}

// check if there is already daemon running
pid_t checkDaemon() {
    char *home = getenv("HOME");
    if(home == NULL) {
        _log(
            ERROR,
            "No HOME environment variable"
        );
        return -1;
    }

    char path_dir[512];
    sprintf(path_dir, "%s/%s/state", home, R_CHERRIES_FOLDER_PULSE);
    mkdir(path_dir, 0755);

    char file_path[1028];

    DIR *dir = opendir(path_dir);
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL) {
        char *name = entry->d_name;
        if(strcmp(name, ".") == 0) continue;
        if(strcmp(name, "..") == 0) continue;
        sprintf(file_path, "%s/%s", path_dir, name);
        
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

pid_t startDaemon(PulseArgs args) {
    _log(
        INFO,
        "Starting Daemon"
    );

    pid_t pid = checkDaemon();
    if(pid > 0) {
        _log(INFO, "Attached to previous Daemon");
        sem_t *ready_sem = sem_open(CHERRIES_PULSE_READY_SEM, 0);
        if (ready_sem == SEM_FAILED) {
            _log(ERROR, "Failed to open ready semaphore");
            exit(EXIT_FAILURE);
        }
        sem_post(ready_sem);
        sem_close(ready_sem);
        
        return pid;
    }

    pid = fork();
    if (pid == 0) {
        _log(INFO, "Setting up Daemon.");
        setupDaemon(getpid());

        sem_t *ready_sem = sem_open(CHERRIES_PULSE_READY_SEM, 0);
        if (ready_sem == SEM_FAILED) {
            _log(ERROR, "Failed to open ready semaphore");
            exit(EXIT_FAILURE);
        }
        sem_post(ready_sem);
        sem_close(ready_sem);


        int shm_fd = shm_open(CHERRIES_PULSE_SHM, O_RDWR, 0);
        if(shm_fd == -1) {
            _log(
                ERROR,
                "SHM open failed"
            );
            exit(EXIT_FAILURE);
        }

        struct shmbuf *shmp;
        shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (shmp == MAP_FAILED) {
            _log(
                ERROR,
                "Mapping object failed"
            );
            exit(EXIT_FAILURE);
        }

        System system_snapshot = getSystem();
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

        // _log(
        //     INFO,
        //     "Now starting sem1 wait."
        // );

        short repetition = 0;
        while(true) {
            if(repetition % 5 == 0) {
                writeHistoryM(metrics);
                writeHistoryS(system_snapshot);
                repetition = 0;
            }

            repetition++;


            prev_system_snapshot = system_snapshot;
            system_snapshot = getSystem();
            metrics = getMetrics(prev_system_snapshot, system_snapshot);

            pthread_mutex_lock(&shmp->lock);
            shmp->metrics = metrics;
            shmp->system = system_snapshot;
            pthread_mutex_unlock(&shmp->lock);

            sleep(args.sleep);
        }

        shm_unlink(CHERRIES_PULSE_SHM);
        exit(EXIT_SUCCESS);
    }

    return pid;
}
