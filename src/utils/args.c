#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "utils.h"

PulseArgs parseArgs(int argc, char* argv[]) {
    PulseArgs p = {
        .web = false,
        .help = false,
        .stop = false,
        .headless = false,
        
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
