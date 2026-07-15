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

void handle(Args args) {
    Command cmd = args.command;

    switch (cmd) {
        case STOP:
            stop();
            break;
        
        case HELP:
            help();
            break;
        
        case MONITOR:
            monitor(args);
            break;
        
        case INFO:
            info(args);
            break;
        
        case TOP:
            top(args);
            break;
        
        default:
            break;
    }
}

void term(int sig) {
    // if(render_pid == getpid()) {
    //     failureLog();
    // }

    running = 0;
    stop();
    exit(0);
}

int main(int argc, char* argv[]) {
    signal(SIGINT, term);

    int s = setup();
    if(s < 0) {
        return -1;
    }
    setupLog();

    Args args = parseArgs(argc, argv);
    handle(args);

    return 0;
}

