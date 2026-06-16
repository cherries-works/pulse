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

struct Disk getDisk(size_t size, char *buffer) {
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
        
        char *next = strchr(line, ' ');
        *next = '\0';
        line = next + 1;
        
        if(SECTORS_WRITTEN != i && SECTORS_READ != i) continue;
        if(SECTORS_READ == i) {
            read = atoi(line);
        } else if (SECTORS_WRITTEN == i) {
            written = atoi(line);
        }
    }


    struct Disk snapshot = { disk_total, disk_available, read, written };
    return snapshot;
}

struct IoAverage parseIoUsage(struct Disk snapshot2, struct Disk snapshot1) {
    float r = snapshot2.read - snapshot1.read;
    float w = snapshot2.write - snapshot1.write;

    struct IoAverage io = {
        r,
        w
    };
    
    return io;
}