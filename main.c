#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>
#include <wait.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"
#include "daemon.h"
#include "log.h"

#include "app.h"
#include "setup.h"

pid_t website_pid = 0;
pid_t daemon_pid = 0;
pid_t render_pid = 0;

void term(int sig) {
    if(render_pid == getpid()) {
        failureLog();
    }

    running = 0;
    stop();
    exit(0);
}

int main(int argc, char* argv[]) {
    render_pid = getpid();
    signal(SIGINT, term);

    int s = setup();
    if(s < 0) {
        return -1;
    }
    setupLog();

    Args args = parseArgs(argc, argv);
    if(args.help) {
        help();
        return 0;
    }

    if(args.stop) {
        stop();
        return 0;
    }

    if(args.command == INFO) {
        info(args);
        return 0;
    } else if(args.command == TOP) {
        top(args);
        return 0;
    }

    // clean up any leftover from a crash
    sem_unlink(CHERRIES_PULSE_READY_SEM);
    sem_t *ready_sem = sem_open(CHERRIES_PULSE_READY_SEM, O_CREAT | O_EXCL, 0600, 0);
    if (ready_sem == SEM_FAILED) {
        _log(L_ERROR, "Failed to create ready semaphore");
        exit(EXIT_FAILURE);
    }

    daemon_pid = startDaemon(args);
    
    // wait until daemon is ready
    sem_wait(ready_sem);
    sem_post(ready_sem);
    sem_close(ready_sem);
    sem_unlink(CHERRIES_PULSE_READY_SEM);

    if(args.headless && !args.web) return 0;

    if(args.web) {
        website_pid = startWebsite(args);
    }

    if(!args.headless) {
        startRender(args);
    }

    return 0;
}