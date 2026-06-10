#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include <dirent.h>

#include "utils.h"
#include "constants.h"
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

unsigned long parseMemoryKey(char *buffer, char *target_key) {
    char *line = buffer;
    char *value = NULL;
    while(*line) {
        // set a pointer to the \n, to the
        // first \name
        //
        char *next = strchr(line, '\n');
        
        // replace \n with a NULL Terminator
        // so (var) line, ends there.
        //
        if(next) *next = '\0';

        char *colon = strchr(line, ':');
        if (colon) {
            // do the same technique by replacing
            // the colon. 
            //  eg. MemTotal:   ... kB\n
            //             \0         \0  <- replaced
            //
            *colon = '\0';
            trim(line);

            // compare the line to the key, as it
            // is now enclosed via the NULL pointer
            if (strcmp(line, target_key) == 0) {
                // move the colon pointer one
                // forward so the NULL pointers
                // are wrapped
                //
                value = colon + 1;

                trim(value);
                break;
            }
        }

        if (next == NULL) break;
        // move the pointer one forward
        // so we skip the NULL pointers
        //
        line = next + 1; 
    }

    if(value == NULL) return 1;
    return atoi(value);
}

unsigned long parseCpuKey(char *buffer, char *target_key) {
    char *line = buffer;
    char *value = NULL;
    while(*line) {
        char *next = strchr(line, '\n');
        if(next) *next = '\0';

        char *space = strchr(line, ' ');
        if (space) {
            *space = '\0';
            trim(line);

            if (strcmp(line, target_key) == 0) {
                value = space + 1;

                trim(value);
                break;
            }
        }

        if (next == NULL) break;
        line = next + 1; 
    }

    if(value == NULL) return 1;
    return atoi(value);
}

struct Cpu getCpu(size_t size, char *buffer) {
    unsigned long user = 0;
    unsigned long nice = 0;
    unsigned long system = 0;
    unsigned long idle = 0;
    unsigned long iowait = 0;
    unsigned long irq = 0;
    unsigned long softirq = 0;
    unsigned long steal = 0;
    unsigned long guest = 0;
    unsigned long guest_nice = 0;

    unsigned number_position = 0;
    unsigned i = 0;
    unsigned number_index = 0;
    char number[16];
    while(i < size) {
        char character = buffer[i];
        if(character == '\n') break;

        if(character == ' ') {
            if(number_index != i) {
                switch (number_position) {
                    case 0:
                        user = atoi(number);
                        break;
                    case 1:
                        nice = atoi(number);
                        break;
                    case 2:
                        system = atoi(number);
                        break;
                    case 3:
                        idle = atoi(number);
                        break;
                    case 4:
                        iowait = atoi(number);
                        break;
                    case 5:
                        irq = atoi(number);
                        break;
                    case 6:
                        softirq = atoi(number);
                        break;
                    case 7:
                        steal = atoi(number);
                        break;
                    case 8:
                        guest = atoi(number);
                        break;
                    case 9:
                        guest_nice = atoi(number);
                        break;
                    
                    default:
                        break;
                }
                number_position++;
            } else i++;
            while(number_index > 0) {
                number[number_index] = '\0';
                number_index--;
            }

            i++;
            continue;
        }

        number[number_index] = character;
        number_index++;
        i++;
    }

    unsigned long user_time = user - guest;                             
    unsigned long nice_time = nice - guest_nice;                         
    unsigned long idle_all_time = idle + iowait;  
    unsigned long system_all_time = system + irq + softirq;
    unsigned long virt_all_time = guest + guest_nice;
    unsigned long total_time = user_time + nice_time + system_all_time + idle_all_time + steal + virt_all_time;

    unsigned long processes = parseCpuKey(buffer, "processes");
    struct Cpu snapshot = { idle_all_time, total_time, processes };
    return snapshot;
}

struct Load getLoad(size_t size, char *buffer) {
    char *line = buffer;
    char *next = strchr(line, SPACE_IN_ASCII);
    *next = '\0';

    float load1 = atof(line);

    line = next + 1;
    next = strchr(line, SPACE_IN_ASCII);
    *next = '\0';

    float load5 = atof(line);

    line = next + 1;
    next = strchr(line, SPACE_IN_ASCII);
    *next = '\0';

    float load15 = atof(line);

    struct Load snapshot = { load1, load5, load15 };
    return snapshot;
}

struct Network getNetwork(size_t size, char *buffer) {
    unsigned long rx = 0;
    unsigned long tx = 0;

    char *line = buffer;
    char *next = strchr(line, '\n');
    line = next + 1;
    next = strchr(line, '\n');
    line = next + 1;
    
    // Inter-|   Receive           |  Transmit                          \n <--- first strchr
    // face  |bytes  ..  multicast|bytes    packets .... compressed     \n <--- second strchr
    //                                                                  So we start at the start of lo
    // lo:   38914   ..  38914     307    0    ....                0
    // enp2s0:    0 ....         0        0       ....               0
    // wlp0s20f3: 183282384 ....         0 18357363   ....       0          0

    next = strchr(line, ':');
    line = next + 1; // <- skip the network face name
    while(*line) {
        // 8 total keys per face (for rx AND tx)
        int KEYS_AMOUNT = 8;
        for(int i = 0; i < KEYS_AMOUNT; i++) {
            while (*line == ' ') line++;

            next = strchr(line, ' ');
            *next = '\0';
            rx += atoi(line);
            line = next + 1;
        }

        for(int i = 0; i < KEYS_AMOUNT; i++) {
            while (*line == ' ') line++;

            next = strchr(line, ' ');
            *next = '\0';
            tx += atoi(line);
            line = next + 1;
        }
        next = strchr(line, '\n');
        line = next + 1;
    }


    struct Network snapshot = { rx, tx };
    return snapshot;
}

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

float parseCpuUsage(struct Cpu snapshot2, struct Cpu snapshot1) {
    float total_time_float = snapshot2.total == snapshot1.total ? (float)snapshot2.total : (float)snapshot2.total - (float)snapshot1.total;
    float idle_all_time_float = snapshot2.idle == snapshot1.idle ? (float)snapshot2.idle : (float)snapshot2.idle - (float)snapshot1.idle;

    float diff_total_idle = total_time_float - idle_all_time_float;
    float cpu_usage = diff_total_idle / total_time_float;
    cpu_usage *= 100;

    return cpu_usage;
}

struct NetworkAverage parseNetworkUsage(struct Network snapshot2, struct Network snapshot1) {
    float rx = snapshot2.rx - snapshot1.rx;
    float tx = snapshot2.tx - snapshot1.tx;

    struct NetworkAverage n = {
        rx,
        tx
    };
    
    return n;
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

void getProcesses(
    size_t size, 
    char *buffer,
    struct Process processes[]
) {
    struct dirent *procEntry;
    DIR *procDir = opendir(PROC_DIR);

    if(procDir == NULL) {
        perror("Cherries Pulse Error :: Reading /proc failed.");
        exit(EXIT_FAILURE);
    }

    char procFileName[32];
    while((procEntry = readdir(procDir)) != NULL) {
        char procName[64];
        int procPid = 0;
        int procRAM = 0;
        int procCPU = 0;
        char *procEntryName = procEntry->d_name;

        int pid = atoi(procEntryName);
        if(pid == 0) continue;
        procPid = pid;

        sprintf(
            procFileName,
            "/proc/%d/status",
            pid
        );
        readFile(procFileName, size, buffer);
        procRAM = parseMemoryKey(buffer, "VmRSS");

        sprintf(
            procFileName,
            "/proc/%d/stat",
            pid
        );
        readFile(procFileName, size, buffer);

        for(int i = 0; i < 14; i++) {
            char *next = strchr(buffer, ' ');
            if(!next) break;;
            buffer = next + 1;
        }

        char *utime = strchr(buffer, ' ');
        if(!utime) continue;
        *utime = '\0';
        procCPU += atoi(buffer);

        buffer = utime + 1;
        char *stime = strchr(buffer, ' ');
        if(!stime) continue;
        *stime = '\0';
        procCPU += atoi(buffer);


        sprintf(
            procFileName,
            "/proc/%d/comm",
            pid
        );
        int nameSize = readFile(procFileName, 64, procName);
        procName[nameSize - 1] = '\0';

        for(int i = 0; i < 3; i++) {
            // struct Process process = processes[i];
            if(processes[i].ram < procRAM) {
                for(int j = 1; j > i; j--) {
                    processes[j] = processes[j - 1];
                }

                processes[i].pid = procPid;
                processes[i].cpu = procCPU;
                processes[i].ram = procRAM;
                strcpy(processes[i].name, procName);

                // processes[i].name = procName;
                break;
            } else {
                continue;
            }
        }

    }

    closedir(procDir);
    return;
}


struct System getSystem() {
    unsigned statBufferSize = BUFFER_ONE_KB * 6;
    char statBuffer[statBufferSize];
    readFile(PROC_STAT_FILE, statBufferSize, statBuffer);
    struct Cpu cpu = getCpu(BUFFER_ONE_KB, statBuffer);

    unsigned memBufferSize = BUFFER_ONE_KB * 2;
    char memBuffer[memBufferSize];

    readFile(PROC_MEM_FILE, memBufferSize, memBuffer);
    unsigned long memoryTotal = parseMemoryKey(memBuffer, "MemTotal");
    readFile(PROC_MEM_FILE, memBufferSize, memBuffer);
    unsigned long memoryAvailable = parseMemoryKey(memBuffer, "MemAvailable");
    struct Memory memory = { memoryTotal, memoryAvailable };
   
    unsigned diskBufferSsize = BUFFER_ONE_KB * 2;
    char diskBuffer[diskBufferSsize];
    readFile(PROC_DISK_FILE, diskBufferSsize, diskBuffer);
    struct Disk disk = getDisk(diskBufferSsize, diskBuffer);

    unsigned loadBufferSize = BUFFER_ONE_KB;
    char loadBuffer[loadBufferSize];
    readFile(PROC_LOAD_FILE, loadBufferSize, loadBuffer);
    struct Load load = getLoad(loadBufferSize, loadBuffer);

    unsigned networkBufferSize = BUFFER_ONE_KB * 8;
    char networkBuffer[networkBufferSize];
    readFile(PROC_NET_FILE, networkBufferSize, networkBuffer);
    struct Network network = getNetwork(networkBufferSize, networkBuffer);

    struct Process processes[3] = {
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

    struct System system = { 
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
