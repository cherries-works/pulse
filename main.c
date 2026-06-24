#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"

#include "app.h"

typedef struct PulseArgs {
    bool web;
    bool help;
    
    int port;
    unsigned sleep;
} PulseArgs;

PulseArgs parseArgs(int argc, char* argv[]) {
    PulseArgs p = {
        .web = false,
        .help = false,
        .sleep = 1,
        .port = 8080,
    };

    for(int i = 0; i < argc; i++) {
        char* arg = argv[i];

        if(strcmp(arg, "--sleep") == 0) {
            if(i == argc - 1) continue;
            unsigned sleep = (unsigned)atoi(argv[i + 1]);
            if(sleep <= 0 || sleep > 60) continue;
            p.sleep = sleep;
        }

        if(strcmp(arg, "--port") == 0) {
            if(i == argc - 1) continue;
            int port = atoi(argv[i + 1]);
            p.web = 1;
            p.port = port;
        }

        if(strcmp(arg, "--web") == 0) {
            p.web = true;
        }

        if(strcmp(arg, "--help") == 0) {
            p.help = true;
            break;
        }
    }


    return p;
}


int main(int argc, char* argv[]) {
    PulseArgs args = parseArgs(argc, argv);
    if(args.help) {
        help();
        return 0;
    }
    
    System systemSnapshot = getSystem();
    System prevSystemSnapshot = systemSnapshot;

    if(args.web) {
        printf("API running on :: %d\n", args.port);
        Route routes[64];
        RouteHandler handler = { 0, routes };
        initRoutes(&handler);

        Server server = serverContsructor(
            &handler,           // route handler
            AF_INET, 
            (uint16_t)args.port,
            SOCK_STREAM,
            0,                  // protocol
            10,                 // backlog
            INADDR_ANY,
            serverLaunch
        );
        
        pthread_t serverThread;
        pthread_create(&serverThread, NULL, serverLaunchThread, &server);
    }

    bool LOOP_STARTED = false;
    short TOTAL_LINES = 15;
    while(true) {
        if(!LOOP_STARTED) {
            LOOP_STARTED = true;
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