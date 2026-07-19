#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>

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
        pid_t proc_pid = 0;
        unsigned long proc_ram = 0;
        unsigned long proc_cpu = 0;
        char *proc_entry_name = proc_entry->d_name;

        pid_t pid = (pid_t)atoi(proc_entry_name);
        if(pid == 0) continue;
        proc_pid = pid;

        snprintf(
            proc_file_name,
            proc_file_name_size,
            "/proc/%d/status",
            pid
        );

        char *cursor = processes_buffer;
        readFile(proc_file_name, processes_buffer_size, processes_buffer);
        proc_ram = parseMemoryKey(processes_buffer, "VmRSS");

        snprintf(
            proc_file_name,
            proc_file_name_size,
            "/proc/%d/stat",
            pid
        );
        readFile(proc_file_name, processes_buffer_size, processes_buffer);
        cursor = processes_buffer;

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

void getProcess(Process *p, pid_t pid) {
    size_t processes_buffer_size = BUFFER_ONE_KB * 8;
    char processes_buffer[processes_buffer_size];
    
    enum {
        keys_until_status = 3,
        keys_until_parent_pid = 4,
        keys_until_utime = 14,
        keys_until_threads = 20,
        keys_until_uptime = 22
    };

    const size_t proc_file_name_size = 32;
    char proc_file_name[proc_file_name_size];
    
    const size_t proc_name_size = 64;
    char proc_name[proc_name_size];

    pid_t proc_pid = 0;
    pid_t proc_parent_pid = 0;
    
    unsigned long proc_uptime = 0;
    unsigned long proc_threads = 0;
    unsigned long proc_ram = 0;
    unsigned long proc_cpu = 0;
    proc_pid = pid;

    snprintf(
        proc_file_name,
        proc_file_name_size,
        "/proc/%d/status",
        proc_pid
    );

    char *cursor = processes_buffer;
    readFile(proc_file_name, processes_buffer_size, processes_buffer);
    proc_ram = parseMemoryKey(processes_buffer, "VmRSS");

    snprintf(
        proc_file_name,
        proc_file_name_size,
        "/proc/%d/stat",
        proc_pid
    );

    readFile(proc_file_name, processes_buffer_size, processes_buffer);
    cursor = processes_buffer;

    char *close_bracket = strchr(cursor, ')');
    if(!close_bracket) {
        printf("NO CLOSE BRACKET FOR PROCESS\n");
        exit(EXIT_FAILURE);
    }

    // skip ) and space.
    cursor = close_bracket + 2;

    for(int i = 2; i < 32; i++) {
        char *next = strchr(cursor, ' ');
        if(!next) break;
        *next = '\0';

        switch ((i + 1)) {
            case keys_until_status: {
                switch (*cursor) {
                    case 'R': p->status = S_RUNNING; break;
                    case 'S': p->status = S_SLEEPING; break;
                    case 'D': p->status = S_DISK_SLEEP; break;
                    case 'Z': p->status = S_ZOMBIE; break;
                    case 'T': p->status = S_STOPPED; break;
                    default:  p->status = S_UNKNOWN; break;
                }

                break;
            }
            case keys_until_parent_pid: {
                proc_parent_pid = atoi(cursor);

                break;
            }
            case keys_until_utime: {
                char *utime = strchr(cursor, ' ');
                if(!utime) break;
                *utime = '\0';
                proc_cpu += strtoull(cursor, NULL, 10);

                cursor = utime + 1;
                char *stime = strchr(cursor, ' ');
                if(!stime) break;
                *stime = '\0';
                proc_cpu += strtoull(cursor, NULL, 10);
                
                break;
            }
            case keys_until_threads: {
                char *threads = strchr(cursor, ' ');
                if(!threads) break;
                *threads = '\0';

                proc_threads = strtoull(cursor, NULL, 10);
                
                break;
            }
            case keys_until_uptime: {
                char *uptime = strchr(cursor, ' ');
                if(!uptime) break;
                *uptime = '\0';
                
                proc_uptime = strtoull(cursor, NULL, 10);

                break;
            }
            default: break;
        }

        cursor = next + 1;
    }

    snprintf(
        proc_file_name,
        proc_file_name_size,
        "/proc/%d/comm",
        proc_pid
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

    p->pid = proc_pid;
    p->parent_pid = proc_parent_pid;

    p->cpu = proc_cpu;
    p->ram = proc_ram;
    p->uptime = proc_uptime;
    p->threads = proc_threads;
    strcpy(p->name, proc_name);
        
    return;
}
