#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "parse.h"

System getSystem(
    PulseArgs args
) {
    size_t stat_buffer_size = BUFFER_ONE_KB * 6;
    char stat_buffer[stat_buffer_size];
    readFile(PROC_STAT_FILE, stat_buffer_size, stat_buffer);
    Cpu cpu = getCpu(BUFFER_ONE_KB, stat_buffer);

    size_t mem_buffer_size = BUFFER_ONE_KB * 2;
    char mem_buffer[mem_buffer_size];

    readFile(PROC_MEM_FILE, mem_buffer_size, mem_buffer);
    unsigned long memory_total = parseMemoryKey(mem_buffer, "MemTotal");
    readFile(PROC_MEM_FILE, mem_buffer_size, mem_buffer);
    unsigned long memory_available = parseMemoryKey(mem_buffer, "MemAvailable");
    Memory memory = { memory_total, memory_available };
   
    size_t disk_buffer_size = BUFFER_ONE_KB * 2;
    char disk_buffer[disk_buffer_size];
    readFile(PROC_DISK_FILE, disk_buffer_size, disk_buffer);
    Disk disk = getDisk(disk_buffer);

    size_t load_buffer_size = BUFFER_ONE_KB;
    char load_buffer[load_buffer_size];
    readFile(PROC_LOAD_FILE, load_buffer_size, load_buffer);
    Load load = getLoad(load_buffer);

    size_t network_buffer_size = BUFFER_ONE_KB * 8;
    char network_buffer[network_buffer_size];
    readFile(PROC_NET_FILE, network_buffer_size, network_buffer);
    Network network = getNetwork(network_buffer);

    size_t uptime_buffer_size = BUFFER_ONE_KB * 2;
    char uptime_buffer[uptime_buffer_size];
    readFile(PROC_UPTIME_FILE, uptime_buffer_size, uptime_buffer);
    unsigned long uptime = parseUptime(uptime_buffer_size, uptime_buffer);

    unsigned temp = parseTemp();

    System system = { 
        .cpu = cpu, 
        .memory = memory, 
        .disk = disk,
        
        .load = load,
        .network = network,
        
        .processes = {},
        .processes_count = args.processes,

        .uptime = uptime,
        .temp = temp
    };

    size_t processes_buffer_size = BUFFER_ONE_KB * 8;
    char processes_buffer[processes_buffer_size];
    getProcesses(
        processes_buffer_size,
        processes_buffer,
        system.processes,
        args
    );

    return system;
}

Metrics getMetrics(System system2, System system1) {
    float cpuUsage = parseCpuUsage(system1.cpu, system2.cpu);

    float memoryTotalFloat = (float)system1.memory.total / 1024.0f / 1024.0f;
    float memoryAvailableFloat = (float)system1.memory.available / 1024.0f / 1024.0f;
    float ramUsage = 100 - (memoryAvailableFloat / memoryTotalFloat) * 100;

    float diskTotalFloat = (float)system1.disk.total / 1024.0f / 1024.0f / 1024.0f;
    float diskAvailableFloat = (float)system1.disk.available / 1024.0f / 1024.0f / 1024.0f;
    float diskUsage = 100 - (diskAvailableFloat / diskTotalFloat) * 100; 

    NetworkAverage networkUsage = parseNetworkUsage(system1.network, system2.network);
    IoAverage ioUsage = parseIoUsage(system1.disk, system2.disk);

    Metrics metrics = {
        .cpuUsage = cpuUsage,
        .ramUsage = ramUsage,
        .diskUsage = diskUsage,
        .rx = networkUsage.rx / 1024.0f,
        .tx = networkUsage.tx / 1024.0f,
        .read = (ioUsage.r * 512.0f) / 1024.0f,
        .write = (ioUsage.w * 512.0f) / 1024.0f
    };

    return metrics;
}
