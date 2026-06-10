#include <stdio.h>

#include "parse.h"
#include "utils.h"

char *RED = "\u001b[31m";
char *GREEN = "\u001b[32m";
char *YELLOW = "\u001b[33m";
char *BLUE = "\u001b[34m";
char *WHITE = "\u001b[37m";

char *BOLD = "\u001b[1m";
char *DIM = "\u001b[2m";
char *ITALIC = "\u001b[3m";
char *UNDERLINE = "\u001b[4m";

char *RESET = "\u001b[0m";

char *ONE_BARS =    "█░░░░░░░░░";
char *TWO_BARS =    "██░░░░░░░░";
char *THREE_BARS =  "███░░░░░░░";
char *FOUR_BARS =   "████░░░░░░";
char *FIVE_BARS =   "█████░░░░░";
char *SIX_BARS =    "██████░░░░";
char *SEVEN_BARS =  "███████░░░";
char *EIGHT_BARS =  "████████░░";
char *NINE_BARS =   "█████████░";
char *TEN_BARS =    "██████████";

char *getBars(float val) {
    if(val < 10.0f) return ONE_BARS;
    if(val < 20.0f) return TWO_BARS;
    if(val < 30.0f) return THREE_BARS;
    if(val < 40.0f) return FOUR_BARS;
    if(val < 50.0f) return FIVE_BARS;
    if(val < 60.0f) return SIX_BARS;
    if(val < 70.0f) return SEVEN_BARS;
    if(val < 80.0f) return EIGHT_BARS;
    if(val < 90.0f) return NINE_BARS;
    return TEN_BARS;
}

const char* getColor(float value) {
    if (value < 60.0f) return GREEN;
    if (value < 80.0f) return YELLOW;
    return RED;
}

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
    struct Process process,
    struct System system
) {
    printf(
        "│ (%-6d) %-15s RAM %8.2f MB %18s CPU %5.2f%%  │\n",
        process.pid,
        process.name,
        
        ((float)process.ram) / 1024.0f,
        "",
        ((float)process.cpu / (float)system.cpu.total) * 100,
        ""
    );
}

void render(
    struct System systemSnapshot, 
    struct System prevSystemSnapshot
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

    struct NetworkAverage networkUsage = parseNetworkUsage(systemSnapshot.network, prevSystemSnapshot.network);
    struct IoAverage ioUsage = parseIoUsage(systemSnapshot.disk, prevSystemSnapshot.disk);

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
        struct Process process = systemSnapshot.processes[i];
        printProcess(process, systemSnapshot);
    }
    printf("└─────────────────────────────────────────────────────────────────────────┘\n");
}

void help() {
    printf("%s%sCherries Pulse%s ───────────────────────────────────── v 0.1 ──── \n\n", BOLD, RED, RESET);
    printf("     %-20s %-20s\n", "--port [number]", "Determine the port where the website will be hosted (omits --web).");
    printf("     %-20s %-20s\n", "--web", "Hosts website (and API) on default port 8080.");
    printf("     %-20s %-20s\n", "--sleep", "How many seconds the program sleeps before updating (TUI only).");
    printf("     %-20s %-20s\n\n", "--help", "Prints this.");

}
