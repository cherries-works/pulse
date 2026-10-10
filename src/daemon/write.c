#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <inttypes.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "utils.h"
#include "daemon.h"
#include "log.h"
#include "setup.h"

void writeHistoryS(System system, struct CWInfo cwinfo) {
    size_t time_buffer_size = BUFFER_ONE_KB / 8;
    char time_buffer[time_buffer_size];
    formatTime(cwinfo.started_at, time_buffer, time_buffer_size);

    time_t _time = time(NULL);

    size_t history_path_size = BUFFER_ONE_KB;
    char history_path[history_path_size];
    snprintf(
        history_path, history_path_size, 
        "%s/history/%s/system/%" PRIu64 "", 
        cwinfo.pulse,
        time_buffer, 
        _time
    );

    FILE *f = fopen(history_path, "w");
    if(f == NULL) {
        _log(cwinfo, L_ERROR, "History path invalid [writeHistoryS].");
        return;
    }

    size_t buffer_size = BUFFER_ONE_KB / 4;
    char buffer[buffer_size];

    snprintf(buffer, buffer_size, "%" PRIu64 "\n", system.cpu.idle);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%" PRIu64 "\n", system.cpu.processes);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%" PRIu64 "\n", system.cpu.total);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%" PRIu64 "\n", system.disk.available);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%" PRIu64 "\n", system.disk.read);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%" PRIu64 "\n", system.disk.total);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%" PRIu64 "\n", system.disk.write);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%f\n", system.load.load1);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%f\n", system.load.load5);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%f\n", system.load.load15);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%" PRIu64 "\n", system.memory.available);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%" PRIu64 "\n", system.memory.total);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%" PRIu64 "\n", system.network.rx);
    fwrite(buffer, strlen(buffer), 1, f);

    snprintf(buffer, buffer_size, "%" PRIu64 "\n", system.network.tx);
    fwrite(buffer, strlen(buffer), 1, f);


    for(unsigned i = 0; i < system.processes_count; i++) {
        snprintf(buffer, buffer_size, "%s\n", system.processes[i].name);
        fwrite(buffer, strlen(buffer), 1, f);
    
        snprintf(buffer, buffer_size, "%" PRIu64 "\n", system.processes[i].cpu);
        fwrite(buffer, strlen(buffer), 1, f);
        
        snprintf(buffer, buffer_size, "%d\n", system.processes[i].pid);
        fwrite(buffer, strlen(buffer), 1, f);
        
        snprintf(buffer, buffer_size, "%" PRIu64 "\n", system.processes[i].ram);
        fwrite(buffer, strlen(buffer), 1, f);
    }

    snprintf(buffer, buffer_size, "%" PRIu64 "\n", system.uptime);
    fwrite(buffer, strlen(buffer), 1, f);
    fclose(f);
}

void writeHistoryM(Metrics metrics, struct CWInfo cwinfo) {
    size_t time_buffer_size = BUFFER_ONE_KB / 8;
    char time_buffer[time_buffer_size];
    formatTime(cwinfo.started_at, time_buffer, time_buffer_size);

    time_t _time = time(NULL);

    size_t history_path_size = BUFFER_ONE_KB;
    char history_path[history_path_size];
    snprintf(
        history_path, history_path_size, 
        "%s/history/%s/metric/%" PRIu64 "", 
        cwinfo.pulse,
        time_buffer,
        _time
    );

    FILE *f = fopen(history_path, "w");
    if(f == NULL) {
        _log(cwinfo, L_ERROR, "History path invalid [writeHistoryM].");
        return;
    }

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

    snprintf(buffer, buffer_size, "%f\n", metrics.tx);
    fwrite(buffer, strlen(buffer), 1, f);

    fclose(f);
}
