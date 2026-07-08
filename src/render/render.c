#include <stdio.h>

#include "parse.h"
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
    PulseArgs args,
    System system,
    Metrics metrics 
) {
    size_t uptime_buffer_text_size = BUFFER_ONE_KB / 8;
    char uptime_buffer_text[uptime_buffer_text_size];
    formatTimeHumanReadable(system.uptime, uptime_buffer_text, uptime_buffer_text_size);
    if(args.web) {
        printf("%s%sCherries Pulse%s ──────────── port :: %d ──────────────── ", BOLD, RED, RESET, args.port);
    } else {
        printf("%s%sCherries Pulse%s ─────────────────────────────────────────── ", BOLD, RED, RESET);
    }
    printf("%-7s %s%-20s%s\n",
        "Uptime:",
        BOLD,
        uptime_buffer_text,
        RESET
    );

    printf("┌── RESOURCES ────────────────────────┐ ┌── SYSTEM LOAD ──────────────────┐\n");
    printMetric("CPU", metrics.cpuUsage, "1 min:", system.load.load1);
    printMetric("RAM", metrics.ramUsage, "5 min:", system.load.load5);
    printMetric("DISK", metrics.diskUsage, "15 min:", system.load.load15);
    printf("└─────────────────────────────────────┘ └─────────────────────────────────┘\n");


    printf("┌── NETWORK ──────────────────────────┐ ┌── DISK I/O ─────────────────────┐\n");
    printTransferRow("RX:", metrics.rx, "READ:", metrics.read);
    printTransferRow("TX:", metrics.tx / 1024.0f, "WRITE:", metrics.write);
    printf("└─────────────────────────────────────┘ └─────────────────────────────────┘\n");


    printf("┌── PROCESSES ────────────────────────────────────────────────────────────┐\n");
    for(unsigned i = 0; i < args.processes; i++) {
        Process process = system.processes[i];
        printProcess(process, system);
    }
    printf("└─────────────────────────────────────────────────────────────────────────┘\n");
}

void help() {
    printf("%s%sCherries Pulse%s ───────────────────────────────────── v0.2.0 ──── \n\n", BOLD, RED, RESET);
    printf("     %-20s %-20s\n", "--port [number]", "Determine the port where the website will be hosted (omits --web).");
    printf("     %-20s %-20s\n", "--web", "Hosts website (and API) on default port 8080.");
    printf("     %-20s %-20s\n", "--sleep", "How many seconds the program sleeps before updating (TUI only).");
    printf("     %-20s %-20s\n\n", "--headless", "Runs program without TUI (currently only useful with --web).");
    printf("     %-20s %-20s\n\n", "--stop", "Stops all of the processes that Pulse currently runs.");
    printf("     %-20s %-20s\n\n", "--processes", "Amount of processes that are being monitored.");
    printf("     %-20s %-20s\n\n", "--sort", "Sorts the processes between \"cpu\" and \"ram\".");
    printf("     %-20s %-20s\n\n", "--help", "Prints this.");
}
