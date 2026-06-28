#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "utils.h"
#include "log.h"

void setupLog() {
    const char *info = "Logging initted.";
    char *home = getenv("HOME");
    if(home == NULL) {
        printf("Error: No HOME enviroment variable...");
        return;
    }
    
    char current_file[1024];
    sprintf(current_file, "%s/%s/state/log", home, R_CHERRIES_FOLDER_PULSE);
    if(access(current_file, F_OK) == 0) {
        _log(
            INFO,
            info
        );
        return;
    }

    time_t _time = time(NULL);
    char log_file[1024];
    sprintf(log_file, "%s/%s/logs/%ld.log", home, R_CHERRIES_FOLDER_PULSE, _time);
    
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
    char buffer[128];
    sprintf(buffer, "%ld", _time);
    fwrite(buffer, strlen(buffer), 1, f);
    fclose(f);


    _log(
        INFO,
        info
    );
}

unsigned long getCurrentLog() {
    char *home = getenv("HOME");
    if(home == NULL) return 0;

    char path_file[1024];
    sprintf(path_file, "%s/%s/state/log", home, R_CHERRIES_FOLDER_PULSE);
    FILE *f = fopen(path_file, "r");
    if(f == NULL) return 0;
    char buffer[128];
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, f);
    fclose(f);
    
    buffer[n] = '\0';
    unsigned long t = strtoul(buffer, NULL, 10);
    return t;
}

void failureLog() {
    char *home = getenv("HOME");
    if(home == NULL) {
        printf("Error: No HOME enviroment variable...");
        return;
    }

    unsigned long t = getCurrentLog();
    char path_file[1024];
    sprintf(path_file, "%s/%s/logs/%ld.log", home, R_CHERRIES_FOLDER_PULSE, t);
    
    printf("Error :: See %s, for logs.\n", path_file);
}


void _log(
    log_types types, 
    const char *message
) {
    unsigned long t = getCurrentLog();
    if(t == 0) {
        printf("No logger set up...\n");
        return;
    }

    char *home = getenv("HOME");
    if(home == NULL) {
        printf("No HOME enviroment variable...");
        return;
    }

    char path_file[1024];
    sprintf(path_file, "%s/%s/logs/%ld.log", home, R_CHERRIES_FOLDER_PULSE, t);

    FILE *f = fopen(path_file, "a");
    if(f == NULL) {
        printf("Opening log file failed.");
        return;
    }

    if(types == INFO) {
        fwrite("[INFO] ", 7, 1, f);
    } else if(types == ERROR) {
        fwrite("[ERROR] ", 8, 1, f);
    }

    fwrite(message, strlen(message), 1, f);
    fwrite("\n", 1, 1, f);
    fclose(f);
}
