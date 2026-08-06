#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "utils.h"
#include "daemon.h"
#include "log.h"

void writeHistoryS(System system) {
    char *home = getenv("HOME");
    if(home == NULL) {
        _log(
            L_ERROR,
            "No HOME environment variable"
        );
        return;
    }

    size_t time_buffer_size = BUFFER_ONE_KB / 8;
    char time_buffer[time_buffer_size];
    time_t log_time = getCurrentLog();
    formatTime(log_time, time_buffer, time_buffer_size);

    time_t _time = time(NULL);

    size_t history_path_size = BUFFER_ONE_KB;
    char history_path[history_path_size];
    snprintf(
        history_path, history_path_size, 
        "%s/%s/history/%s/system/%ld", 
        home, 
        R_CHERRIES_FOLDER_PULSE, 
        time_buffer, 
        _time
    );

    FILE *f = fopen(history_path, "a");
    
    size_t buffer_size = BUFFER_ONE_KB / 4;
    char buffer[buffer_size];

    snprintf(buffer, buffer_size, "%ld\n", system.cpu.idle);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%ld\n", system.cpu.processes);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%ld\n", system.cpu.total);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%lld\n", system.disk.available);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%lld\n", system.disk.read);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%lld\n", system.disk.total);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%lld\n", system.disk.write);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%f\n", system.load.load1);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%f\n", system.load.load5);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%f\n", system.load.load15);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%ld\n", system.memory.available);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%ld\n", system.memory.total);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%ld\n", system.network.rx);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%ld\n", system.network.tx);
    fwrite(buffer, strlen(buffer), 1, f);


    for(unsigned i = 0; i < system.processes_count; i++) {
        snprintf(buffer, buffer_size, "%s\n", system.processes[i].name);
        fwrite(buffer, strlen(buffer), 1, f);
    
        snprintf(buffer, buffer_size, "%ld\n", system.processes[i].cpu);
        fwrite(buffer, strlen(buffer), 1, f);
        
        snprintf(buffer, buffer_size, "%d\n", system.processes[i].pid);
        fwrite(buffer, strlen(buffer), 1, f);
        
        snprintf(buffer, buffer_size, "%ld\n", system.processes[i].ram);
        fwrite(buffer, strlen(buffer), 1, f);
    }

    snprintf(buffer, buffer_size, "%ld", system.uptime);
    fwrite(buffer, strlen(buffer), 1, f);

    fclose(f);
}

void writeHistoryM(Metrics metrics) {
    char *home = getenv("HOME");
    if(home == NULL) {
        _log(
            L_ERROR,
            "No HOME environment variable"
        );
        return;
    }

    size_t time_buffer_size = BUFFER_ONE_KB / 8;
    char time_buffer[time_buffer_size];
    time_t log_time = getCurrentLog();
    formatTime(log_time, time_buffer, time_buffer_size);

    time_t _time = time(NULL);

    size_t history_path_size = BUFFER_ONE_KB;
    char history_path[history_path_size];
    snprintf(
        history_path, history_path_size, 
        "%s/%s/history/%s/metric/%ld", 
        home, 
        R_CHERRIES_FOLDER_PULSE, 
        time_buffer,
        _time
    );

    FILE *f = fopen(history_path, "a");

    size_t buffer_size = BUFFER_ONE_KB / 4;
    char buffer[buffer_size];

    snprintf(buffer, buffer_size, "%f\n", metrics.cpuUsage);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%f\n", metrics.diskUsage);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%f\n", metrics.ramUsage);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%f\n", metrics.read);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%f\n", metrics.write);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%f\n", metrics.rx);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%f", metrics.tx);
    fwrite(buffer, strlen(buffer), 1, f);

    fclose(f);
}
