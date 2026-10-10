#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "utils.h"
#include "log.h"
#include "setup.h"

void _log(
    struct CWInfo cwinfo,
    log_types type, 
    const char *message
) {
    size_t log_time_buffer_size = BUFFER_ONE_KB / 8;
    char log_time_buffer[log_time_buffer_size];
    formatTime(cwinfo.started_at, log_time_buffer, log_time_buffer_size);

    char path_file[BUFFER_ONE_KB];
    int _snprintf = snprintf(path_file, BUFFER_ONE_KB, "%s/%s.log", cwinfo.log, log_time_buffer);
    if(_snprintf == -1) return;

    FILE *f = fopen(path_file, "a");
    if(f == NULL) {
        printf("Opening log file failed (%s).\n", path_file);
        return;
    }

    time_t t = time(NULL);
    char formatted_time[128];
    formatTime(t, formatted_time, 128);

    size_t _fwrite = fwrite(formatted_time, strlen(formatted_time), 1, f);
    if(_fwrite == 0) return;
    if(type == L_INFO) {
        _fwrite = fwrite(" [INFO] ", 8, 1, f);
    } else if(type == L_ERROR) {
        _fwrite = fwrite(" [ERROR] ", 9, 1, f);
    }
    if(_fwrite == 0) return;

    _fwrite = fwrite(message, strlen(message), 1, f);
    if(_fwrite == 0) return;
    
    _fwrite = fwrite("\n", 1, 1, f);
    if(_fwrite == 0) return;
    
    int _fclose = fclose(f);
    if(_fclose == -1) return;
    return;
}
