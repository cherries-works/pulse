#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#include "parse.h"
#include "render.h"

#include "app.h"
#include "log.h"

void setupWebsite(pid_t pid) {
    char *home = getenv("HOME");
    if(home == NULL) {
        _log(ERROR, "No HOME environment variable");
        return;
    }

    char path_dir[BUFFER_ONE_KB / 2];
    snprintf(path_dir, BUFFER_ONE_KB / 2, "%s/%s/state", home, R_CHERRIES_FOLDER_PULSE);
    mkdir(path_dir, 0755);

    char website_path[BUFFER_ONE_KB];
    snprintf(website_path, BUFFER_ONE_KB, "%s/website.%d", path_dir, pid);

    // create website file
    FILE *f = fopen(website_path, "a");
    fclose(f);
}

// check if there is already daemon running
pid_t checkWebsite() {
    char *home = getenv("HOME");
    if(home == NULL) {
        _log(
            ERROR,
            "No HOME environment variable"
        );
        return -1;
    }

    char path_dir[BUFFER_ONE_KB / 2];
    snprintf(path_dir, BUFFER_ONE_KB / 2, "%s/%s/state", home, R_CHERRIES_FOLDER_PULSE);
    mkdir(path_dir, 0755);

    char file_path[BUFFER_ONE_KB];

    DIR *dir = opendir(path_dir);
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL) {
        char *name = entry->d_name;
        if(strcmp(name, ".") == 0) continue;
        if(strcmp(name, "..") == 0) continue;
        snprintf(file_path, BUFFER_ONE_KB, "%s/%s", path_dir, name);
        
        char *d = strchr(name, '.');
        if(d == NULL) continue;
        *d = '\0';

        if(strcmp(name, "website") != 0) continue;
        name = d + 1;
        int pid = atoi(name);

        closedir(dir);
        return pid;
    }

    closedir(dir);
    return -1;
}


pid_t startWebsite(Args args) {
    _log(INFO, "Starting Website");

    pid_t pid = checkWebsite();
    if(pid > 0) return pid;

    pid = fork();

    if (pid == 0) {
        setupWebsite(getpid());
        Route routes[64];
        RouteHandler handler = { 0, routes }; 
        initRoutes(&handler); 

        Server server = serverContsructor( 
            &handler, // route handler 
            AF_INET, 
            args.port,
            SOCK_STREAM, 
            0, // protocol 
            10, // backlog 
            INADDR_ANY, 
            serverLaunch 
        );

        serverLaunch(&server);
        exit(EXIT_SUCCESS);
    }

    return pid;
}
