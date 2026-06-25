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

#include "app.h"
#include "setup.h"

void stop() {
    char *home = getenv("HOME");
    if(home == NULL) {
        printf("Error: No HOME enviroment variable...");
        return;
    }

    char path_dir[512];
    sprintf(path_dir, "%s/%s", home, R_CHERRIES_FOLDER_PULSE);
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
        name = d + 1;
        int pid = atoi(name);
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
        
        remove(file_path);
    }

    closedir(dir);
}

void term(int sig) {
    running = 0;
    stop();
    exit(0);
}

int main(int argc, char* argv[]) {    
    signal(SIGTERM, term);
    int s = setup();
    if(s < 0) {
        printf("Failed to run setup.\n");
        return -1;
    }

    PulseArgs args = parseArgs(argc, argv);
    if(args.help) {
        help();
        return 0;
    }

    if(args.stop) {
        stop();
        return 0;
    }
    
    startDaemon(args);
    if(args.headless && !args.web) return 0;

    sleep(1); // to prevent shm_open failing 
    if(args.web) {
        startWebsite(args);
    }

    if(!args.headless) {
        startRender(args);
    }

    return 0;
}