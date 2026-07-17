#ifndef PARSE_H
#define PARSE_H

#include "utils.h"

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

typedef enum {
    RUNNING,
    SLEEPING,
    ZOMBIE,
} Status;

typedef struct {
    pid_t pid;
    pid_t parent_pid;

    unsigned long time;

    unsigned long ram;
    unsigned long cpu;
    unsigned long threads;
    
    Status status;

    char exec[256];
    char name[64];
} Process;

typedef struct {
    char release[256];
    char machine[256];
    char sysname[256];
} Kernel;

typedef struct {
    char os[256];
    char hostname[256];
    char cpu_model[256];
    char desktop[256];
    char session[256];
    
    Kernel kernel;
    unsigned cores;
} Info;

#define MAX_PROCESSES 100

typedef struct {
    Cpu cpu;
    Memory memory;
    Disk disk;
    
    Load load;
    Network network;

    Process processes[MAX_PROCESSES];
    unsigned processes_count;

    unsigned long uptime;
    unsigned temp;
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
extern System getSystem(Args args);
extern Metrics getMetrics(System system2, System system1);
extern Info getInfo();

extern unsigned long parseMemoryKey(char *buffer, char *target_key);
extern unsigned long parseUptime(size_t size, char *buffer);
extern unsigned parseTemp();

extern float parseCpuUsage(Cpu snapshot2, Cpu snapshot1);
extern void getProcesses(Process processes[], Args args);
extern void getProcess(Process *process, pid_t pid);

#endif