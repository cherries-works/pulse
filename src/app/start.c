#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <stdbool.h>

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"

#include "app.h"
#include "setup.h"
#include "app.h"

void setupWebsite(pid_t pid) {
    char *home = getenv("HOME");
    if(home == NULL) {
        printf("Error: No HOME enviroment variable...");
        return;
    }

    char path_dir[512];
    sprintf(path_dir, "%s/%s", home, R_CHERRIES_FOLDER_PULSE);
    mkdir(path_dir, 0755);

    char website_path[1028];
    sprintf(website_path, "%s/website.%d", path_dir, pid);

    // create website file
    FILE *f = fopen(website_path, "a");
    fclose(f);
}


void startWebsite(PulseArgs args) {
    pid_t pid = fork();

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

    return;
}