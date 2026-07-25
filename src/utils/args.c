#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>

#include "utils.h"
#include "log.h"

const char *commands[] = {
    "MONITOR",
    "TOP",
    "INFO",
    "STOP",
    "HELP",
    "PROCESS",
    "PRUNE"
};

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
        .processes = 3,
        .process = 0,

        .prune = ALL,
        .keep = 0,
        .until = "3000-00-00"
    };

    if(argc > 1) {
        char *arg = argv[1];
        if(!startsWith(arg, strlen(arg), "--", 2)) {
            if(strcmp(arg, "monitor") == 0) p.command = MONITOR;
            else if(strcmp(arg, "info") == 0) p.command = INFO;
            else if(strcmp(arg, "help") == 0) p.command = HELP;
            else if(strcmp(arg, "top") == 0) p.command = TOP;
            else if(strcmp(arg, "stop") == 0) p.command = STOP;
            else if(strcmp(arg, "process") == 0) p.command = PROCESS;
            else if(strcmp(arg, "prune") == 0) p.command = PRUNE;
            else {
                printf("Invalid command.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    char log_buffer[256];
    snprintf(log_buffer, 256, "Running command %s", commands[p.command]);
    _log(
        L_INFO,
        log_buffer
    );

    for(int i = 0; i < argc; i++) {
        char* arg = argv[i];

        if(strcmp(arg, "--sleep") == 0) {
            if(i == argc - 1) continue;
            unsigned sleep = (unsigned)atoi(argv[i + 1]);
            if(sleep <= 0 || sleep > 60) continue;
            p.sleep = sleep;
        }

        if(strcmp(arg, "--keep") == 0) {
            if(i == argc - 1) continue;
            unsigned keep = (unsigned)atoi(argv[i + 1]);
            if(keep <= 0) continue;
            p.keep = keep;
        }

        if(strcmp(arg, "--until") == 0) {
            if(i == argc - 1) continue;
            strcpy(p.until, argv[i + 1]);
        }

        if(strcmp(arg, "--prune") == 0) {
            if(i == argc - 1) continue;
            if(strcmp("history", argv[i + 1]) == 0) {
                p.prune = HISTORY;
            } else if(strcmp("logs", argv[i + 1]) == 0) {
                p.prune = LOGS;
            }
        }

        if(strcmp(arg, "--processes") == 0) {
            if(i == argc - 1) continue;
            unsigned processes = (unsigned)atoi(argv[i + 1]);
            if(p.command == MONITOR) {
                if(processes <= 0 || processes > 10) continue;
            } else if(p.command == TOP) {
                if(processes <= 0 || processes > 100) continue;
            }
            p.processes = processes;
        }

        if(strcmp(arg, "--process") == 0) {
            if(i == argc - 1) continue;
            pid_t process = (pid_t)atoi(argv[i + 1]);
            p.process = process;
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

    if(p.command == MONITOR) {
        if(p.headless) {
            _log(
                L_INFO,
                "Running headless mode."
            );
        }
        if(p.web) {
            _log(
                L_INFO,
                "Running website."
            );
        }
    }

    return p;
}
