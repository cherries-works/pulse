#ifndef PARSE_H
#define PARSE_H

#include "utils.h"
#include <fcntl.h>

typedef struct {
    uint64_t idle;
    uint64_t total;
    uint64_t processes;
} Cpu;

typedef struct {
    uint64_t total;
    uint64_t available;
} Memory;

typedef struct {
    uint64_t total;
    uint64_t available;
    uint64_t read;
    uint64_t write;
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
    uint64_t rx;
    uint64_t tx;
} Network;

typedef struct {
    float rx;
    float tx;
} NetworkAverage;

typedef enum {
    S_RUNNING,
    S_SLEEPING,
    S_ZOMBIE,
    S_STOPPED,
    S_DISK_SLEEP,
    S_UNKNOWN
} Status;

typedef struct {
    pid_t pid;
    pid_t parent_pid;

    uint64_t uptime;
    uint64_t ram;
    uint64_t cpu;
    uint64_t threads;
    
    Status status;

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
    unsigned process_count;

    uint64_t uptime;
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
extern Memory getMemory(size_t size, char *buffer);
extern Network getNetwork(size_t size, char *buffer);
extern Load getLoad(size_t size, char *buffer);
extern Disk getDisk(size_t size, char *buffer);
extern System getSystem(Args args);
extern Metrics getMetrics(System system2, System system1);
extern Info getInfo();

extern uint64_t parseMemoryKey(char *buffer, char *target_key);
extern uint64_t parseUptime(size_t size, char *buffer);
extern unsigned parseTemp();

extern float parseCpuUsage(Cpu snapshot2, Cpu snapshot1);
extern unsigned getProcesses(Process processes[], Args args);
extern void getProcess(Process *process, pid_t pid);

#endif
