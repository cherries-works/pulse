#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "utils.h"
#include "parse.h"

void getProcesses(
    Process processes[],
    Args args
) {
    size_t processes_buffer_size = BUFFER_ONE_KB * 8;
    char processes_buffer[processes_buffer_size];
    
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

        char *cursor = processes_buffer;
        readFile(proc_file_name, processes_buffer_size, cursor);
        proc_ram = parseMemoryKey(cursor, "VmRSS");

        snprintf(
            proc_file_name,
            proc_file_name_size,
            "/proc/%d/stat",
            pid
        );
        readFile(proc_file_name, processes_buffer_size, cursor);

        for(int i = 0; i < keys_until_utime; i++) {
            char *next = strchr(cursor, ' ');
            if(!next) break;
            cursor = next + 1;
        }

        char *utime = strchr(cursor, ' ');
        if(!utime) continue;
        *utime = '\0';
        proc_cpu += strtoull(cursor, NULL, 10);

        cursor = utime + 1;
        char *stime = strchr(cursor, ' ');
        if(!stime) continue;
        *stime = '\0';
        proc_cpu += strtoull(cursor, NULL, 10);

        snprintf(
            proc_file_name,
            proc_file_name_size,
            "/proc/%d/comm",
            pid
        );

        size_t name_size = readFile(proc_file_name, proc_name_size, proc_name);
        // the names wrap 16 characters total whilst printing.
        size_t max_name_size = 16;
        if(name_size >= max_name_size) {
            // -4 because of ... and \0 (4 bytes)
            for(size_t i = max_name_size - 4; i < max_name_size; i++) {
                proc_name[i] = '.';
            }
            proc_name[max_name_size - 1] = '\0';
        } else {
            proc_name[name_size - 1] = '\0';
        }

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
