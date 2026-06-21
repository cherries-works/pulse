#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include <dirent.h>

#include "utils.h"
#include "parse.h"

// reading the first number
unsigned long parseUptime(size_t size, char *buffer) {
    unsigned i = 0;

    // should be maximum 16 characters
    char number[16];
    while(i < size) {
        char c = buffer[i];
        if(c == '\0') {
            break;
        }
        if(c == ' ') {
            break;
        }

        number[i] = c;
        i++;
    }

    unsigned long n = atoi(number);
    return n;
}



System getSystem() {
    unsigned statBufferSize = BUFFER_ONE_KB * 6;
    char statBuffer[statBufferSize];
    readFile(PROC_STAT_FILE, statBufferSize, statBuffer);
    Cpu cpu = getCpu(BUFFER_ONE_KB, statBuffer);

    unsigned memBufferSize = BUFFER_ONE_KB * 2;
    char memBuffer[memBufferSize];

    readFile(PROC_MEM_FILE, memBufferSize, memBuffer);
    unsigned long memoryTotal = parseMemoryKey(memBuffer, "MemTotal");
    readFile(PROC_MEM_FILE, memBufferSize, memBuffer);
    unsigned long memoryAvailable = parseMemoryKey(memBuffer, "MemAvailable");
    Memory memory = { memoryTotal, memoryAvailable };
   
    unsigned diskBufferSsize = BUFFER_ONE_KB * 2;
    char diskBuffer[diskBufferSsize];
    readFile(PROC_DISK_FILE, diskBufferSsize, diskBuffer);
    Disk disk = getDisk(diskBufferSsize, diskBuffer);

    unsigned loadBufferSize = BUFFER_ONE_KB;
    char loadBuffer[loadBufferSize];
    readFile(PROC_LOAD_FILE, loadBufferSize, loadBuffer);
    Load load = getLoad(loadBufferSize, loadBuffer);

    unsigned networkBufferSize = BUFFER_ONE_KB * 8;
    char networkBuffer[networkBufferSize];
    readFile(PROC_NET_FILE, networkBufferSize, networkBuffer);
    Network network = getNetwork(networkBufferSize, networkBuffer);

    Process processes[3] = {
        { "", 0, 0, 0 },
        { "", 0, 0, 0 },
        { "", 0, 0, 0 }
    };

    unsigned processesBufferSize = BUFFER_ONE_KB * 8;
    char processesBuffer[processesBufferSize];
    getProcesses(processesBufferSize, processesBuffer, processes);

    unsigned uptimeBufferSize = BUFFER_ONE_KB * 2;
    char uptimeBuffer[uptimeBufferSize];
    readFile(PROC_UPTIME_FILE, uptimeBufferSize, uptimeBuffer);
    unsigned long uptime = parseUptime(uptimeBufferSize, uptimeBuffer);

    System system = { 
        cpu, 
        memory, 
        disk,
        
        load,
        network,
        
        {
            processes[0],
            processes[1],
            processes[2]
        },

        uptime 
    };

    return system;
}
