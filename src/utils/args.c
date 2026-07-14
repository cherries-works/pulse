#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "utils.h"
#include "log.h"

Args parseArgs(int argc, char* argv[]) {
    _log(
        INFO,
        "Parsing CLI arguments"
    );

    Args p = {
        .web = false,
        .help = false,
        .stop = false,
        .headless = false,

        .command = monitor,
        .sort = RAM,
        .sleep = 1,
        .port = 8080,
        .processes = 3
    };

    if(argc > 1) {
        char *arg = argv[1];
        if(strcmp(arg, "info") == 0) p.command = info;
        if(strcmp(arg, "top") == 0) p.command = top;
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

        if(strcmp(arg, "--stop") == 0) {
            p.stop = true;
        }
     
        if(strcmp(arg, "--headless") == 0) {
            p.headless = true;
        }

        if(strcmp(arg, "--help") == 0) {
            p.help = true;
            break;
        }
    }

    return p;
}
