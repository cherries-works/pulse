#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "utils.h"
#include "parse.h"

void getProcesses(
    size_t size, 
    char *buffer, 
    Process processes[],
    Args args
) {
    struct dirent *proc_entry;
    DIR *proc_dir = opendir(PROC_DIR);

    if(proc_dir == NULL) {
        perror("Cherries Pulse Error :: Reading /proc failed.");
        exit(EXIT_FAILURE);
    }

    int keys_until_utime = 14;
    
    size_t proc_file_name_size = 32;
    char proc_file_name[proc_file_name_size];
    
    size_t proc_name_size = 64;
    char proc_name[proc_name_size];
    while((proc_entry = readdir(proc_dir)) != NULL) {
        unsigned proc_pid = 0;
        unsigned long proc_ram = 0;
        unsigned long proc_cpu = 0;
        char *proc_entry_name = proc_entry->d_name;

        unsigned pid = (unsigned)atoi(proc_entry_name);
        if(pid == 0) continue;
        proc_pid = pid;

        snprintf(
            proc_file_name,
            proc_file_name_size,
            "/proc/%d/status",
            pid
        );
        readFile(proc_file_name, size, buffer);
        proc_ram = parseMemoryKey(buffer, "VmRSS");

        snprintf(
            proc_file_name,
            proc_file_name_size,
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


        snprintf(
            proc_file_name,
            proc_name_size,
            "/proc/%d/comm",
            pid
        );

        size_t name_size = readFile(proc_file_name, proc_name_size, proc_name);
        proc_name[name_size - 1] = '\0';

        for(unsigned i = 0; i < args.processes; i++) {
            bool t = processes[i].ram < proc_ram;
            if(args.sort == CPU) t = processes[i].cpu < proc_cpu;

            if(t) {
                for(unsigned j = 1; j > i; j--) {
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
