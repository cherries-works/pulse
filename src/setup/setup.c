#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>

#include "utils.h"
#include "log.h"
#include "setup.h"

struct CWInfo setup() {
    struct CWInfo cwinfo = {
        .home = "",
        .pulse = "",
        .log = "",
        .history = "",
        .started_at = time(NULL),
    };

    char *home = getenv("HOME");
    if(home == NULL) {
        return cwinfo;
    }

    char path[BUFFER_ONE_KB];
    snprintf(path, BUFFER_ONE_KB, "%s/%s", home, R_CHERRIES_FOLDER);
    strcpy(cwinfo.home, path);

    DIR *dir = opendir(cwinfo.home);
    if(!dir) {
        mkdir(cwinfo.home, 0755);
    } else {
        closedir(dir);
    }

    snprintf(path, BUFFER_ONE_KB, "%s/%s", home, R_CHERRIES_FOLDER_PULSE);
    strcpy(cwinfo.pulse, path);

    dir = opendir(path);
    if(!dir) {
        mkdir(path, 0755);
    } else {
        closedir(dir);
    }

    snprintf(path, BUFFER_ONE_KB, "%s/config.toml", cwinfo.pulse);
    if(access(path,F_OK) != 0) {
        int fd = creat(path, 0644);

        ssize_t result = write(fd, CHERRIES_DEFAULT_TOML, sizeof(CHERRIES_DEFAULT_TOML));
        if(result <= 0) return cwinfo;
    }

    snprintf(path, BUFFER_ONE_KB, "%s/state", cwinfo.pulse);
    dir = opendir(path);
    if(!dir) {
        mkdir(path, 0755);
    } else {
        closedir(dir);
    }

    snprintf(path, BUFFER_ONE_KB, "%s/logs", cwinfo.pulse);
    strcpy(cwinfo.log, path);

    dir = opendir(path);
    if(!dir) {
        mkdir(path, 0755);
    } else {
        closedir(dir);
    }

    snprintf(path, BUFFER_ONE_KB, "%s/history", cwinfo.pulse);
    strcpy(cwinfo.history, path);

    dir = opendir(path);
    if(!dir) {
        mkdir(path, 0755);
    } else {
        closedir(dir);
    }

    return cwinfo;
}

