#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <signal.h>

#include "utils.h"
#include "log.h"

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
    snprintf(path, BUFFER_ONE_KB, "%s/%s", home, R_CHERRIES_FOLDER);

    DIR *dir = opendir(path);
    if(!dir) {
        mkdir(path, 0755);
    } else {
        closedir(dir);
    }

    
    snprintf(path, BUFFER_ONE_KB, "%s/%s", home, R_CHERRIES_FOLDER_PULSE);
    dir = opendir(path);
    if(!dir) {
        mkdir(path, 0755);
    } else {
        closedir(dir);
    }
    
    snprintf(path, BUFFER_ONE_KB, "%s/%s/state", home, R_CHERRIES_FOLDER_PULSE);
    dir = opendir(path);
    if(!dir) {
        mkdir(path, 0755);
    } else {
        closedir(dir);
    }
    
    snprintf(path, BUFFER_ONE_KB, "%s/%s/logs", home, R_CHERRIES_FOLDER_PULSE);
    dir = opendir(path);
    if(!dir) {
        mkdir(path, 0755);
    } else {
        closedir(dir);
    }

    snprintf(path, BUFFER_ONE_KB, "%s/%s/history", home, R_CHERRIES_FOLDER_PULSE);
    dir = opendir(path);
    if(!dir) {
        mkdir(path, 0755);
    } else {
        closedir(dir);
    }

    return 0;
}

