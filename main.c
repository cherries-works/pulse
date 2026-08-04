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
#include <termios.h>

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"
#include "daemon.h"
#include "log.h"

#include "app.h"
#include "setup.h"
#include "config.h"

void handle(Args args, Config _config) {
    Command cmd = args.command;

    switch (cmd) {
        case STOP:
            stop();
            break;
        
        case HELP:
            help();
            break;
        
        case MONITOR:
            monitor(args, _config);
            break;
        
        case INFO:
            info(args);
            break;
        
        case TOP:
            top(args);
            break;
        
        case PROCESS:
            process(args);
            break;
        
        case PRUNE:
            prune(args);
            break;

        case CONFIG:
            config();
            break;
        
        default:
            break;
    }
}

pid_t render_pid = 0;
struct termios oldt;
void term(int sig) {
    if(getpid() == render_pid) {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        failureLog();
    }

    stop();
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    render_pid = getpid();

    tcgetattr(STDIN_FILENO, &oldt);
    signal(SIGINT, term);

    int s = setup();
    if(s < 0) {
        return -1;
    }
    setupLog();

    Args args = parseArgs(argc, argv);
    Config config = parseToml();
    handle(args, config);

    return 0;
}

