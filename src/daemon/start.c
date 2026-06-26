#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <stdbool.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"

#include "app.h"
#include "utils.h"
#include "daemon.h"

#include <sys/socket.h>
#include <sys/un.h>

void setupDaemon(pid_t pid) {
    char *home = getenv("HOME");
    if(home == NULL) {
        printf("Error: No HOME enviroment variable...");
        return;
    }

    char path_dir[512];
    sprintf(path_dir, "%s/%s/state", home, R_CHERRIES_FOLDER_PULSE);
    mkdir(path_dir, 0755);

    char daemon_path[1028];
    sprintf(daemon_path, "%s/daemon.%d", path_dir, pid);

    FILE *f = fopen(daemon_path, "a");
    fclose(f);

    shm_unlink("/cherries_pulse");
    int shm_fd = shm_open("/cherries_pulse", O_CREAT | O_EXCL | O_RDWR, 0600);
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
    int fd = shm_open("/cherries_pulse", O_RDWR, 0);
    if(fd == -1) {
        printf("SHM open failed.\n");
        exit(EXIT_FAILURE);
    }

    struct shmbuf *shmp;
    shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        printf("Mapping object failed.\n");
        exit(EXIT_FAILURE);
    }

    memcpy(metrics, &shmp->metrics, sizeof(Metrics));
    memcpy(system, &shmp->system, sizeof(System));
    /* Tell peer that it can now access shared memory.  */
    if (sem_post(&shmp->sem1) == -1) {
        printf("Posting access to modifieable data failed (sem 1).\n");
        exit(EXIT_FAILURE);
    }

    /* Wait until peer says that it has finished accessing
        the shared memory.  */
    if (sem_wait(&shmp->sem2) == -1) {
        printf("Posting access finished failed (sem 2).\n");
        exit(EXIT_FAILURE);
    }
}


void readDaemonS(System *system) {
    int fd = shm_open("/cherries_pulse", O_RDWR, 0);
    if(fd == -1) {
        printf("SHM open failed.\n");
        exit(EXIT_FAILURE);
    }

    struct shmbuf *shmp;
    shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        printf("Mapping object failed.\n");
        exit(EXIT_FAILURE);
    }

    memcpy(system, &shmp->system, sizeof(System));
    /* Tell peer that it can now access shared memory.  */
    if (sem_post(&shmp->sem1) == -1) {
        printf("Posting access to modifieable data failed (sem 1).\n");
        exit(EXIT_FAILURE);
    }

    /* Wait until peer says that it has finished accessing
        the shared memory.  */
    if (sem_wait(&shmp->sem2) == -1) {
        printf("Posting access finished failed (sem 2).\n");
        exit(EXIT_FAILURE);
    }
}

void readDaemonM(Metrics *metrics) {
    int fd = shm_open("/cherries_pulse", O_RDWR, 0);
    if(fd == -1) {
        printf("SHM open failed.\n");
        exit(EXIT_FAILURE);
    }

    struct shmbuf *shmp;
    shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        printf("Mapping object failed.\n");
        exit(EXIT_FAILURE);
    }

    memcpy(metrics, &shmp->metrics, sizeof(Metrics));
    /* Tell peer that it can now access shared memory.  */
    if (sem_post(&shmp->sem1) == -1) {
        printf("Posting access to modifieable data failed (sem 1).\n");
        exit(EXIT_FAILURE);
    }

    /* Wait until peer says that it has finished accessing
        the shared memory.  */
    if (sem_wait(&shmp->sem2) == -1) {
        printf("Posting access finished failed (sem 2).\n");
        exit(EXIT_FAILURE);
    }
}

// check if there is already daemon running
pid_t check() {
    char *home = getenv("HOME");
    if(home == NULL) {
        printf("Error: No HOME enviroment variable...");
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
    pid_t pid = check();
    if(pid > 0) return;

    pid = fork();

    if (pid == 0) {
        setupDaemon(getpid());

        int shm_fd = shm_open("/cherries_pulse", O_RDWR, 0);
        if(shm_fd == -1) {
            printf("DAEMON SHM open failed.\n");
            exit(EXIT_FAILURE);
        }

        struct shmbuf *shmp;
        shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (shmp == MAP_FAILED) {
            printf("Mapping object failed.\n");
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
            printf("Posting access to modifieable data failed (sem 2).\n");
            exit(EXIT_FAILURE);
        }

        while(true) {
            if (sem_wait(&shmp->sem1) == -1) {
                printf("Waiting semaphore 1 failed.\n");
                exit(EXIT_FAILURE);
            }

            prev_system_snapshot = system_snapshot;
            system_snapshot = getSystem();
    
            Metrics metrics = getMetrics(prev_system_snapshot, system_snapshot);

            shmp->metrics = metrics;
            shmp->system = system_snapshot;

            if (sem_post(&shmp->sem2) == -1) {
                printf("Posting access to modifieable data failed (sem 2).\n");
                exit(EXIT_FAILURE);
            }

            sleep(args.sleep);
        }

        shm_unlink("/cherries_pulse");
        exit(EXIT_SUCCESS);
    }

    return;
}
