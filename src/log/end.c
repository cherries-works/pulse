#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "utils.h"
#include "log.h"
#include "setup.h"

int printLogFile(struct CWInfo cwinfo) {
    _log(cwinfo, L_INFO, "Exiting...");

    size_t log_time_buffer_size = BUFFER_ONE_KB / 8;
    char log_time_buffer[log_time_buffer_size];
    formatTime(cwinfo.started_at, log_time_buffer, log_time_buffer_size);
    
    char path_file[BUFFER_ONE_KB];
    int _snprintf = snprintf(path_file, BUFFER_ONE_KB, "%s/logs/%s.log", cwinfo.pulse, log_time_buffer);
    if(_snprintf == -1) return -1;

    printf("Exited :: %s\n", path_file);
    return 0;
}
