#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/un.h>

#include "utils.h"
#include "daemon.h"

void testReadDaemonS() {
    System system;
    int fd = shm_open(CHERRIES_PULSE_SHM, O_RDWR, 0);
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

    pthread_mutex_lock(&shmp->lock);
    memcpy(&system, &shmp->system, sizeof(System));
    pthread_mutex_unlock(&shmp->lock);

    printf("%lld\n", system.cpu.idle);
    printf("%lld\n", system.disk.available);
}

void testReadDaemonM() {
    Metrics metrics;
    int fd = shm_open(CHERRIES_PULSE_SHM, O_RDWR, 0);
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

    pthread_mutex_lock(&shmp->lock);
    memcpy(&metrics, &shmp->metrics, sizeof(Metrics));
    pthread_mutex_unlock(&shmp->lock);

    printf("%f\n", metrics.cpuUsage);
    printf("%f\n", metrics.diskUsage);
}

// only for testing purposes
int main() {
    testReadDaemonS();
    testReadDaemonM();
}
