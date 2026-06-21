#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/statvfs.h>
#include <netinet/in.h>
#include <time.h>

#include "utils.h"
#include "parse.h"
#include "http.h"

Disk getDisk(char *buffer) {
    struct statvfs stat;
    statvfs("/", &stat);

    unsigned long long disk_total = (unsigned long long)stat.f_blocks * stat.f_frsize;
    unsigned long long disk_available = (unsigned long long)stat.f_bavail * stat.f_frsize;

    unsigned long read = 0;
    unsigned long written = 0;

    char *line = buffer;

    //  nvme0n1 41230 7077 4819387 43278 ... 0 3400608 1777 2509 930
    int SECTORS_READ = 6;
    int SECTORS_WRITTEN = 10;
    for(int i = 0; i <= SECTORS_WRITTEN; i++) {
        while (*line == ' ') line++;
        if(*line == '\0' || *line == '\n') break;

        char *next = strchr(line, ' ');
        if(next) *next = '\0';
        else line = line + strlen(line);

    
        if(SECTORS_READ == i) {
            read = strtoull(line, NULL, 10);
        } else if (SECTORS_WRITTEN == i) {
            written = strtoull(line, NULL, 10);
        }

        if(next) {
            line = next + 1;
        }
    }


    Disk snapshot = { disk_total, disk_available, read, written };
    return snapshot;
}

IoAverage parseIoUsage(Disk snapshot2, Disk snapshot1) {
    float r = (float)snapshot2.read - (float)snapshot1.read;
    float w = (float)snapshot2.write - (float)snapshot1.write;

    IoAverage io = {
        r,
        w
    };
    
    return io;
}