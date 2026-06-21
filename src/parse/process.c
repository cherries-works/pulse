#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <time.h>
#include <dirent.h>

#include "utils.h"
#include "parse.h"
#include "http.h"

void getProcesses(size_t size, char *buffer, Process processes[]) {
    struct dirent *procEntry;
    DIR *procDir = opendir(PROC_DIR);

    if(procDir == NULL) {
        perror("Cherries Pulse Error :: Reading /proc failed.");
        exit(EXIT_FAILURE);
    }

    char procFileName[32];
    while((procEntry = readdir(procDir)) != NULL) {
        char procName[64];
        int procPid = 0;
        int procRAM = 0;
        int procCPU = 0;
        char *procEntryName = procEntry->d_name;

        int pid = atoi(procEntryName);
        if(pid == 0) continue;
        procPid = pid;

        sprintf(
            procFileName,
            "/proc/%d/status",
            pid
        );
        readFile(procFileName, size, buffer);
        procRAM = parseMemoryKey(buffer, "VmRSS");

        sprintf(
            procFileName,
            "/proc/%d/stat",
            pid
        );
        readFile(procFileName, size, buffer);

        for(int i = 0; i < 14; i++) {
            char *next = strchr(buffer, ' ');
            if(!next) break;;
            buffer = next + 1;
        }

        char *utime = strchr(buffer, ' ');
        if(!utime) continue;
        *utime = '\0';
        procCPU += atoi(buffer);

        buffer = utime + 1;
        char *stime = strchr(buffer, ' ');
        if(!stime) continue;
        *stime = '\0';
        procCPU += atoi(buffer);


        sprintf(
            procFileName,
            "/proc/%d/comm",
            pid
        );
        int nameSize = readFile(procFileName, 64, procName);
        procName[nameSize - 1] = '\0';

        for(int i = 0; i < 3; i++) {
            if(processes[i].ram < procRAM) {
                for(int j = 1; j > i; j--) {
                    processes[j] = processes[j - 1];
                }

                processes[i].pid = procPid;
                processes[i].cpu = procCPU;
                processes[i].ram = procRAM;
                strcpy(processes[i].name, procName);
                break;
            } else {
                continue;
            }
        }

    }

    closedir(procDir);
    return;
}
