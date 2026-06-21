#include <stdio.h>

#include "parse.h"
#include "utils.h"
#include "render.h"

void printMetric(
    const char* label,
    float usage,
    const char* loadLabel,
    float loadValue
) {
    printf(
        "│ %-5s %s%s%5.1f%% %s%s             │ │ %-7s %.2f                    │\n",
        label,
        getColor(usage),
        BOLD,
        usage,
        getBars(usage),
        RESET,
        loadLabel,
        loadValue
    );
}

void printTransferRow(
    const char* leftLabel,
    float leftValue,
    const char* rightLabel,
    float rightValue
) {
    printf(
        "│ %-3s %7.2f KB/s                    │ │ %-7s %7.2f KB/s            │\n",
        leftLabel,
        leftValue,
        rightLabel,
        rightValue
    );
}

void printProcess(
    Process process,
    System system
) {
    printf(
        "│ (%-6d) %-15s RAM %8.2f MB %18s CPU %5.2f%%  │\n",
        process.pid,
        process.name,
        ((float)process.ram) / 1024.0f,
        "",
        ((float)process.cpu / (float)system.cpu.total) * 100
    );
}

void render(
    System systemSnapshot, 
    System prevSystemSnapshot
) {
    char uptimeBufferText[64];
    convertTimeInSecondsToString(systemSnapshot.uptime, uptimeBufferText);
    printf("%s%sCherries Pulse%s ─────────────────────────────────────────── ", BOLD, RED, RESET);
    printf("%-7s %s%-20s%s\n",
        "Uptime:",
        BOLD,
        uptimeBufferText,
        RESET
    );

    float cpuUsage = parseCpuUsage(systemSnapshot.cpu, prevSystemSnapshot.cpu);

    float memoryTotalFloat = systemSnapshot.memory.total / 1024.0f / 1024.0f;
    float memoryAvailableFloat = systemSnapshot.memory.available / 1024.0f / 1024.0f;
    float ramUsage = 100 - (memoryAvailableFloat / memoryTotalFloat) * 100;

    float diskTotalFloat = systemSnapshot.disk.total / 1024.0f / 1024.0f / 1024.0f;
    float diskAvailableFloat = systemSnapshot.disk.available / 1024.0f / 1024.0f / 1024.0f;
    float diskUsage = 100 - (diskAvailableFloat / diskTotalFloat) * 100; 

    NetworkAverage networkUsage = parseNetworkUsage(systemSnapshot.network, prevSystemSnapshot.network);
    IoAverage ioUsage = parseIoUsage(systemSnapshot.disk, prevSystemSnapshot.disk);

    printf("┌── RESOURCES ────────────────────────┐ ┌── SYSTEM LOAD ──────────────────┐\n");
    printMetric("CPU", cpuUsage, "1 min:", systemSnapshot.load.load1);
    printMetric("RAM", ramUsage, "5 min:", systemSnapshot.load.load5);
    printMetric("DISK", diskUsage, "15 min:", systemSnapshot.load.load15);
    printf("└─────────────────────────────────────┘ └─────────────────────────────────┘\n");


    printf("┌── NETWORK ──────────────────────────┐ ┌── DISK I/O ─────────────────────┐\n");
    printTransferRow("RX:", networkUsage.rx / 1024.0f, "READ:", (ioUsage.r * 512.0f) / 1024.0f);
    printTransferRow("TX:", networkUsage.tx / 1024.0f, "WRITE:", (ioUsage.w * 512.0f) / 1024.0f);
    printf("└─────────────────────────────────────┘ └─────────────────────────────────┘\n");


    printf("┌── PROCESSES ────────────────────────────────────────────────────────────┐\n");
    for(int i = 0; i < 3; i++) {
        Process process = systemSnapshot.processes[i];
        printProcess(process, systemSnapshot);
    }
    printf("└─────────────────────────────────────────────────────────────────────────┘\n");
}

void help() {
    printf("%s%sCherries Pulse%s ───────────────────────────────────── v0.1.0 ──── \n\n", BOLD, RED, RESET);
    printf("     %-20s %-20s\n", "--port [number]", "Determine the port where the website will be hosted (omits --web).");
    printf("     %-20s %-20s\n", "--web", "Hosts website (and API) on default port 8080.");
    printf("     %-20s %-20s\n", "--sleep", "How many seconds the program sleeps before updating (TUI only).");
    printf("     %-20s %-20s\n\n", "--help", "Prints this.");

}
