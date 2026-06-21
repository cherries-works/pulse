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
    struct dirent *proc_entry;
    DIR *proc_dir = opendir(PROC_DIR);

    if(proc_dir == NULL) {
        perror("Cherries Pulse Error :: Reading /proc failed.");
        exit(EXIT_FAILURE);
    }

    int keys_until_utime = 14;
    int processes_amount = 3;
    
    size_t proc_file_name_size = 32;
    char proc_file_name[proc_file_name_size];
    
    size_t proc_name_size = 64;
    char proc_name[proc_name_size];
    while((proc_entry = readdir(proc_dir)) != NULL) {
        int proc_pid = 0;
        unsigned long proc_ram = 0;
        unsigned long proc_cpu = 0;
        char *proc_entry_name = proc_entry->d_name;

        int pid = atoi(proc_entry_name);
        if(pid == 0) continue;
        proc_pid = pid;

        sprintf(
            proc_file_name,
            "/proc/%d/status",
            pid
        );
        readFile(proc_file_name, size, buffer);
        proc_ram = parseMemoryKey(buffer, "VmRSS");

        sprintf(
            proc_file_name,
            "/proc/%d/stat",
            pid
        );
        readFile(proc_file_name, size, buffer);

        for(int i = 0; i < keys_until_utime; i++) {
            char *next = strchr(buffer, ' ');
            if(!next) break;;
            buffer = next + 1;
        }

        char *utime = strchr(buffer, ' ');
        if(!utime) continue;
        *utime = '\0';
        proc_cpu += strtoull(buffer, NULL, 10);

        buffer = utime + 1;
        char *stime = strchr(buffer, ' ');
        if(!stime) continue;
        *stime = '\0';
        proc_cpu += strtoull(buffer, NULL, 10);


        sprintf(
            proc_file_name,
            "/proc/%d/comm",
            pid
        );

        size_t name_size = readFile(proc_file_name, proc_name_size, proc_name);
        proc_name[name_size - 1] = '\0';

        for(int i = 0; i < processes_amount; i++) {
            if(processes[i].ram < proc_ram) {
                for(int j = 1; j > i; j--) {
                    processes[j] = processes[j - 1];
                }

                processes[i].pid = proc_pid;
                processes[i].cpu = proc_cpu;
                processes[i].ram = proc_ram;
                strcpy(processes[i].name, proc_name);
                break;
            } else {
                continue;
            }
        }

    }

    closedir(proc_dir);
    return;
}
