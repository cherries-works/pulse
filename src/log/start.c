#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "utils.h"
#include "log.h"

void setupLog() {
    char *home = getenv("HOME");
    if(home == NULL) {
        printf("No HOME enviroment variable...");
        return;
    }

    char current_file[BUFFER_ONE_KB];
    snprintf(current_file, BUFFER_ONE_KB, "%s/%s/state/log", home, R_CHERRIES_FOLDER_PULSE);
    if(access(current_file, F_OK) == 0) {
        _log(
            L_INFO,
            "Logging initted."
        );
        return;
    }

    time_t _time = time(NULL);

    size_t log_time_buffer_size = BUFFER_ONE_KB / 8;
    char log_time_buffer[log_time_buffer_size];
    formatTime(_time, log_time_buffer, log_time_buffer_size);

    char log_file[BUFFER_ONE_KB];
    snprintf(log_file, BUFFER_ONE_KB, "%s/%s/logs/%s.log", home, R_CHERRIES_FOLDER_PULSE, log_time_buffer);

    FILE *f = fopen(log_file, "a");
    if(f == NULL) {
        printf("Creating log file failed.");
        return;
    }
    fclose(f);

    // save current time info
    f = fopen(current_file, "w");
    if(f == NULL) {
        printf("Creating log file failed.");
        return;
    }

    char buffer[BUFFER_ONE_KB / 8];
    snprintf(buffer, BUFFER_ONE_KB / 8, "%ld", _time);
    fwrite(buffer, strlen(buffer), 1, f);
    fclose(f);

    _log(
        L_INFO,
        "Logging initted."
    );
}

time_t getCurrentLog() {
    char *home = getenv("HOME");
    if(home == NULL) {
        _log(
            L_ERROR,
            "No HOME environment variable"
        );
        return 0;
    }

    char path_file[BUFFER_ONE_KB];
    snprintf(path_file, BUFFER_ONE_KB, "%s/%s/state/log", home, R_CHERRIES_FOLDER_PULSE);
    FILE *f = fopen(path_file, "r");
    if(f == NULL) return 0;

    char buffer[128];
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, f);
    fclose(f);

    buffer[n] = '\0';
    time_t t = (time_t)strtoul(buffer, NULL, 10);
    return t;
}

void failureLog() {
    char *home = getenv("HOME");
    if(home == NULL) {
        printf("No HOME enviroment variable...");
        return;
    }

    time_t _time = getCurrentLog();

    size_t log_time_buffer_size = BUFFER_ONE_KB / 8;
    char log_time_buffer[log_time_buffer_size];
    formatTime(_time, log_time_buffer, log_time_buffer_size);
    
    char path_file[BUFFER_ONE_KB];
    snprintf(path_file, BUFFER_ONE_KB, "%s/%s/logs/%s.log", home, R_CHERRIES_FOLDER_PULSE, log_time_buffer);

    printf("Exited :: %s\n", path_file);
}


void _log(
    log_types types, 
    const char *message
) {
    time_t _time = getCurrentLog();
    if(_time == 0) {
        printf("No logger set up...\n");
        return;
    }

    char *home = getenv("HOME");
    if(home == NULL) {
        printf("No HOME enviroment variable...");
        return;
    }

    size_t log_time_buffer_size = BUFFER_ONE_KB / 8;
    char log_time_buffer[log_time_buffer_size];
    formatTime(_time, log_time_buffer, log_time_buffer_size);

    char path_file[BUFFER_ONE_KB];
    snprintf(path_file, BUFFER_ONE_KB, "%s/%s/logs/%s.log", home, R_CHERRIES_FOLDER_PULSE, log_time_buffer);

    FILE *f = fopen(path_file, "a");
    if(f == NULL) {
        printf("Opening log file failed.");
        return;
    }

    time_t t = time(NULL);
    char formatted_time[128];
    formatTime(t, formatted_time, 128);

    fwrite(formatted_time, strlen(formatted_time), 1, f);
    if(types == L_INFO) {
        fwrite(" [INFO] ", 8, 1, f);
    } else if(types == L_ERROR) {
        fwrite(" [ERROR] ", 9, 1, f);
    }

    fwrite(message, strlen(message), 1, f);
    fwrite("\n", 1, 1, f);
    fclose(f);
}
