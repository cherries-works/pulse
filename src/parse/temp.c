#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "parse.h"

char *PRIORITIES[5] = {
    "x86_pkg_temp",
    "TCPU",
    "cpu_thermal",
    "soc_thermal",
    "acpitz"
};

unsigned parseTemp() {
    size_t file_name_size = BUFFER_ONE_KB / 4;
    char file_name[file_name_size];

    size_t file_buffer_size = BUFFER_ONE_KB / 4;
    char file_buffer[file_buffer_size];

    char types[10][BUFFER_ONE_KB / 4];
    for(int i = 0; i < 10; i++) {
        snprintf(
            file_name,
            file_name_size - 1,
            "%s%d/type",
            SYS_THERMAL_ZONE,
            i
        );

        FILE *file_sys = fopen(file_name, "r");
        if(file_sys == NULL) continue;

        size_t file_buffer_n = fread(file_buffer, 1, file_buffer_size - 1, file_sys);
        file_buffer[file_buffer_n] = '\0';
        file_buffer[strcspn(file_buffer, "\n")] = '\0';

        strcpy(types[i], file_buffer);
        fclose(file_sys);
    }

    for(int j = 0; j < 5; j++) {
        for(int i = 0; i < 10; i++) {
            char *type = types[i];
            if(strcmp(type, PRIORITIES[j]) != 0) continue;

            snprintf(
                file_name,
                file_name_size - 1,
                "%s%d/temp",
                SYS_THERMAL_ZONE,
                i
            );

            FILE *file_sys = fopen(file_name, "r");
            if(file_sys == NULL) continue;

            size_t file_buffer_n = fread(file_buffer, 1, file_buffer_size - 1, file_sys);

            file_buffer[file_buffer_n] = '\0';
            file_buffer[strcspn(file_buffer, "\n")] = '\0';

            fclose(file_sys);
            
            int temp = atoi(file_buffer);
            return (unsigned)temp;
        }
    }

    return 0;
}

