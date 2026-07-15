#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "utils.h"
#include "log.h"

Args parseArgs(int argc, char* argv[]) {
    _log(
        L_INFO,
        "Parsing CLI arguments"
    );

    Args p = {
        .web = false,
        .headless = false,

        .command = MONITOR,
        .sort = RAM,
        .sleep = 1,
        .port = 8080,
        .processes = 3
    };

    if(argc > 1) {
        char *arg = argv[1];
        if(!startsWith(arg, strlen(arg), "--", 2)) {
            if(strcmp(arg, "monitor") == 0) p.command = MONITOR;
            else if(strcmp(arg, "info") == 0) p.command = INFO;
            else if(strcmp(arg, "help") == 0) p.command = HELP;
            else if(strcmp(arg, "top") == 0) p.command = TOP;
            else if(strcmp(arg, "stop") == 0) p.command = STOP;
            else {
                printf("Invalid command.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    for(int i = 0; i < argc; i++) {
        char* arg = argv[i];

        if(strcmp(arg, "--sleep") == 0) {
            if(i == argc - 1) continue;
            unsigned sleep = (unsigned)atoi(argv[i + 1]);
            if(sleep <= 0 || sleep > 60) continue;
            p.sleep = sleep;
        }

        if(strcmp(arg, "--processes") == 0) {
            if(i == argc - 1) continue;
            unsigned processes = (unsigned)atoi(argv[i + 1]);
            if(processes <= 0 || processes > 10) continue;
            p.processes = processes;
        }

        if(strcmp(arg, "--sort") == 0) {
            if(i == argc - 1) continue;
            if(strcmp("cpu", argv[i + 1]) == 0) {
                p.sort = CPU;
            }
        }

        if(strcmp(arg, "--port") == 0) {
            if(i == argc - 1) continue;
            int port = atoi(argv[i + 1]);
            p.web = 1;
            p.port = (uint16_t)port;
        }

        if(strcmp(arg, "--web") == 0) {
            p.web = true;
        }
     
        if(strcmp(arg, "--headless") == 0) {
            p.headless = true;
        }
    }

    return p;
}
