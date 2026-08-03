#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>

#include "utils.h"
#include "log.h"

int setup() {
    char *home = getenv("HOME");
    if(home == NULL) {
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

    snprintf(path, BUFFER_ONE_KB, "%s/%s/config.toml", home, R_CHERRIES_FOLDER_PULSE);
    if(access(path,F_OK) != 0) {
        int fd = creat(path, 0644);

        char conf[BUFFER_ONE_KB];
        size_t n = readFile("./src/config/default.toml", BUFFER_ONE_KB, conf);
        write(fd, conf, n);
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

