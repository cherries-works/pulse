#ifndef PARSE_H
#define PARSE_H

typedef struct {
    unsigned long idle;
    unsigned long total;
    unsigned long processes;
} Cpu;

typedef struct {
    unsigned long total;
    unsigned long available;
} Memory;

typedef struct {
    unsigned long long total;
    unsigned long long available;
    unsigned long long read;
    unsigned long long write;
} Disk;

typedef struct {
    float r;
    float w;
} IoAverage;

typedef struct {
    float load1;
    float load5;
    float load15;
} Load;

typedef struct {
    unsigned long rx;
    unsigned long tx;
} Network;

typedef struct {
    float rx;
    float tx;
} NetworkAverage;

typedef struct {
    char name[64];
    int pid;
    
    unsigned long ram;
    unsigned long cpu;
} Process;

typedef struct {
    Cpu cpu;
    Memory memory;
    Disk disk;
    
    Load load;
    Network network;

    Process processes[3];

    unsigned long uptime;
} System;

typedef struct {
    float cpuUsage;
    float ramUsage;
    float diskUsage;
    float rx;
    float tx;
    float read;
    float write;
} Metrics;


extern NetworkAverage parseNetworkUsage(Network snapshot2, Network snapshot1);
extern IoAverage parseIoUsage(Disk snapshot2, Disk snapshot1);

extern Cpu getCpu(size_t size, char *buffer);
extern Network getNetwork(char *buffer);
extern Load getLoad(char *buffer);
extern Disk getDisk(char *buffer);
extern System getSystem();
extern Metrics getMetrics(System system2, System system1);

extern unsigned long parseMemoryKey(char *buffer, char *target_key);
extern float parseCpuUsage(Cpu snapshot2, Cpu snapshot1);
extern void getProcesses(size_t size, char *buffer, Process processes[]);

#endif