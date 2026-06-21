#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"

#include "app.h"

typedef struct PulseArgs {
    int web;
    int help;
    unsigned short sleep;
    unsigned short port;
} PulseArgs;

PulseArgs parseArgs(int argc, char* argv[]) {
    PulseArgs p = {
        .web = 0,
        .help = 0,
        .sleep = 1,
        .port = 8080,
    };

    for(int i = 0; i < argc; i++) {
        char* arg = argv[i];

        if(strcmp(arg, "--sleep") == 0) {
            if(i == argc - 1) continue;
            unsigned sleep = atoi(argv[i + 1]);
            if(sleep <= 0 || sleep > 60) continue;
            p.sleep = sleep;
        }

        if(strcmp(arg, "--port") == 0) {
            if(i == argc - 1) continue;
            unsigned port = atoi(argv[i + 1]);
            p.web = 1;
            p.port = port;
        }

        if(strcmp(arg, "--web") == 0) {
            p.web = 1;
        }

        if(strcmp(arg, "--help") == 0) {
            p.help = 1;
            break;
        }
    }


    return p;
}


int main(int argc, char* argv[]) {
    PulseArgs args = parseArgs(argc, argv);
    if(args.help == 1) {
        help();
        return 0;
    }
    
    System systemSnapshot = getSystem();
    System prevSystemSnapshot = systemSnapshot;

    if(args.web == 1) {
        printf("API running on :: %d\n", args.port);
        Route routes[64];
        RouteHandler handler = { 0, routes };
        initRoutes(&handler);

        Server server = serverContsructor(
            &handler,           // route handler
            AF_INET, 
            args.port,
            SOCK_STREAM,
            0,                  // protocol
            10,                 // backlog
            INADDR_ANY,
            serverLaunch
        );
        
        pthread_t serverThread;
        pthread_create(&serverThread, NULL, serverLaunchThread, &server);
    }

    unsigned short TRUTHY_LOOP = 1;
    unsigned short LOOP_STARTED = 0;
    unsigned short TOTAL_LINES = 15;
    while(TRUTHY_LOOP) {
        if(LOOP_STARTED == 0) {
            LOOP_STARTED = 1;
        } else {
            clearLines(TOTAL_LINES);
        }
        
        prevSystemSnapshot = systemSnapshot;
        systemSnapshot = getSystem();

        render(systemSnapshot, prevSystemSnapshot);
        sleep(args.sleep);
    }

    return 0;
}