#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

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
        _log(
            ERROR,
            "No HOME environment variable"
        );
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

    if (sem_init(&shmp->sem1, 1, 0) == -1) {
        exit(EXIT_FAILURE);
    }
    if (sem_init(&shmp->sem2, 1, 0) == -1) {
        exit(EXIT_FAILURE);
    }

    if (sem_wait(&shmp->sem1) == -1) {
        exit(EXIT_FAILURE);
    }
}

void readDaemonSM(System *system, Metrics *metrics) {
    int fd = shm_open(CHERRIES_PULSE_SHM, O_RDWR, 0);
    if(fd == -1) {
        _log(
            ERROR,
            "SHM open failed"
        );
        exit(EXIT_FAILURE);
    }

    struct shmbuf *shmp;
    shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        _log(
            ERROR,
            "Mapping object failed"
        );
        exit(EXIT_FAILURE);
    }

    memcpy(metrics, &shmp->metrics, sizeof(Metrics));
    memcpy(system, &shmp->system, sizeof(System));
    /* Tell peer that it can now access shared memory.  */
    if (sem_post(&shmp->sem1) == -1) {
        _log(
            ERROR,
            "Posting access via sem1 failed"
        );
        exit(EXIT_FAILURE);
    }

    /* Wait until peer says that it has finished accessing
        the shared memory.  */
    if (sem_wait(&shmp->sem2) == -1) {
        _log(
            ERROR,
            "Posting access via sem2 failed"
        );
        exit(EXIT_FAILURE);
    }
}


void readDaemonS(System *system) {
    int fd = shm_open(CHERRIES_PULSE_SHM, O_RDWR, 0);
    if(fd == -1) {
        _log(
            ERROR,
            "SHM open failed"
        );
        exit(EXIT_FAILURE);
    }

    struct shmbuf *shmp;
    shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        _log(
            ERROR,
            "Mapping object failed"
        );
        exit(EXIT_FAILURE);
    }

    memcpy(system, &shmp->system, sizeof(System));
    /* Tell peer that it can now access shared memory.  */
    if (sem_post(&shmp->sem1) == -1) {
        _log(
            ERROR,
            "Posting access via sem1 failed"
        );
        exit(EXIT_FAILURE);
    }

    /* Wait until peer says that it has finished accessing
        the shared memory.  */
    if (sem_wait(&shmp->sem2) == -1) {
        _log(
            ERROR,
            "Posting access via sem2 failed"
        );
        exit(EXIT_FAILURE);
    }
}

void readDaemonM(Metrics *metrics) {
    int fd = shm_open(CHERRIES_PULSE_SHM, O_RDWR, 0);
    if(fd == -1) {
        _log(
            ERROR,
            "SHM open failed"
        );
        exit(EXIT_FAILURE);
    }

    struct shmbuf *shmp;
    shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        _log(
            ERROR,
            "Mapping object failed"
        );
        exit(EXIT_FAILURE);
    }

    memcpy(metrics, &shmp->metrics, sizeof(Metrics));
    /* Tell peer that it can now access shared memory.  */
    if (sem_post(&shmp->sem1) == -1) {
        _log(
            ERROR,
            "Posting access via sem1 failed"
        );
        exit(EXIT_FAILURE);
    }

    /* Wait until peer says that it has finished accessing
        the shared memory.  */
    if (sem_wait(&shmp->sem2) == -1) {
        _log(
            ERROR,
            "Posting access via sem2 failed"
        );
        exit(EXIT_FAILURE);
    }
}

// check if there is already daemon running
pid_t check() {
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

void startDaemon(PulseArgs args) {
    _log(
        INFO,
        "Starting Daemon"
    );


    pid_t pid = check();
    if(pid > 0) {
        _log(
            INFO,
            "Attached to previous Daemon"
        );

        return;
    }

    pid = fork();

    if (pid == 0) {
        setupDaemon(getpid());

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

        /* Post 'sem2' to tell the peer that it can now
            access the modified data in shared memory.  */
        if (sem_post(&shmp->sem2) == -1) {
            _log(
                ERROR,
                "Posting access via sem1 failed"
            );
            exit(EXIT_FAILURE);
        }

        while(true) {
            if (sem_wait(&shmp->sem1) == -1) {
                _log(
                    ERROR,
                    "Waiting for sem1 failed"
                );
                exit(EXIT_FAILURE);
            }

            prev_system_snapshot = system_snapshot;
            system_snapshot = getSystem();
    
            Metrics metrics = getMetrics(prev_system_snapshot, system_snapshot);

            shmp->metrics = metrics;
            shmp->system = system_snapshot;

            if (sem_post(&shmp->sem2) == -1) {
                _log(
                    ERROR,
                    "Posting access via sem1 failed"
                );
                exit(EXIT_FAILURE);
            }

            sleep(args.sleep);
        }

        shm_unlink(CHERRIES_PULSE_SHM);
        exit(EXIT_SUCCESS);
    }

    return;
}
