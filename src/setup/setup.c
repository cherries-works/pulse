#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "utils.h"
#include "daemon.h"
#include "log.h"

volatile sig_atomic_t running = 1;

int setup() {
    // _log(
    //     INFO,
    //     "Setting up Pulse"
    // );

    char *home = getenv("HOME");
    if(home == NULL) {
        // _log(
        //     ERROR,
        //     "No HOME environment variable"
        // );
        return -1;
    }

    char path[BUFFER_ONE_KB];
    sprintf(path, "%s/%s", home, R_CHERRIES_FOLDER);

    DIR *dir = opendir(path);
    if(!dir) {
        mkdir(path, 0755);
    } else {
        closedir(dir);
    }

    
    sprintf(path, "%s/%s", home, R_CHERRIES_FOLDER_PULSE);
    dir = opendir(path);
    if(!dir) {
        mkdir(path, 0755);
    } else {
        closedir(dir);
    }
    
    sprintf(path, "%s/%s/state", home, R_CHERRIES_FOLDER_PULSE);
    dir = opendir(path);
    if(!dir) {
        mkdir(path, 0755);
    } else {
        closedir(dir);
    }
    
    sprintf(path, "%s/%s/logs", home, R_CHERRIES_FOLDER_PULSE);
    dir = opendir(path);
    if(!dir) {
        mkdir(path, 0755);
    } else {
        closedir(dir);
    }

    sprintf(path, "%s/%s/history", home, R_CHERRIES_FOLDER_PULSE);
    dir = opendir(path);
    if(!dir) {
        mkdir(path, 0755);
    } else {
        closedir(dir);
    }

    return 0;
}

