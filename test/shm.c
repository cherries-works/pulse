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

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"

#include "app.h"
#include "setup.h"
#include "utils.h"
#include "daemon.h"

#include <sys/socket.h>
#include <sys/un.h>

void testReadDaemonS() {
    System system;
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

    memcpy(&system, &shmp->system, sizeof(System));
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

    printf("%lld\n", system.cpu.idle);
    printf("%lld\n", system.disk.available);
}

void testReadDaemonM() {
    Metrics metrics;
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

    memcpy(&metrics, &shmp->metrics, sizeof(Metrics));
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

    printf("%f\n", metrics.cpuUsage);
    printf("%f\n", metrics.diskUsage);
}

// only for testing purposes
int main() {
    testReadDaemonS();
    testReadDaemonM();
}
