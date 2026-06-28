#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#include "parse.h"
#include "render.h"

#include "app.h"
#include "log.h"

void setupWebsite(pid_t pid) {
    char *home = getenv("HOME");
    if(home == NULL) {
        _log(
            ERROR,
            "No HOME environment variable"
        );
        return;
    }

    char path_dir[512];
    sprintf(path_dir, "%s/%s/state", home, R_CHERRIES_FOLDER_PULSE);
    mkdir(path_dir, 0755);

    char website_path[1028];
    sprintf(website_path, "%s/website.%d", path_dir, pid);

    // create website file
    FILE *f = fopen(website_path, "a");
    fclose(f);
}

pid_t startWebsite(PulseArgs args) {
    _log(
        INFO,
        "Starting Website"
    );

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