#ifndef PARSE_H
#define PARSE_H

struct Cpu {
    unsigned long idle;
    unsigned long total;
    unsigned long processes;
};

struct Memory {
    unsigned long total;
    unsigned long available;
};

struct Disk {
    unsigned long long total;
    unsigned long long available;
    unsigned long long read;
    unsigned long long write;
};

struct IoAverage {
    float r;
    float w;
};

struct Load {
    float load1;
    float load5;
    float load15;
};

struct Network {
    unsigned long rx;
    unsigned long tx;
};

struct NetworkAverage {
    float rx;
    float tx;
};

struct Process {
    char name[64];
    int pid;
    
    unsigned long ram;
    unsigned long cpu;
};

struct System {
    struct Cpu cpu;
    struct Memory memory;
    struct Disk disk;
    
    struct Load load;
    struct Network network;

    struct Process processes[3];

    unsigned long uptime;
};

extern struct NetworkAverage parseNetworkUsage(struct Network snapshot2, struct Network snapshot1);
extern struct IoAverage parseIoUsage(struct Disk snapshot2, struct Disk snapshot1);

extern struct Cpu getCpu(size_t size, char *buffer);
extern struct Network getNetwork(size_t size, char *buffer);
extern struct Load getLoad(size_t size, char *buffer);
extern struct Disk getDisk(size_t size, char *buffer);
extern struct System getSystem();

extern unsigned long parseMemoryKey(char *buffer, char *target_key);
extern float parseCpuUsage(struct Cpu snapshot2, struct Cpu snapshot1);
extern void getProcesses(size_t size, char *buffer, struct Process processes[]);

#endif