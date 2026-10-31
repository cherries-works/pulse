#include <stdio.h>
#include <inttypes.h>

#include "parse.h"
#include "render.h"
#include "os.h"

void disableColors(void) {
    RED       = "";
    GREEN     = "";
    YELLOW    = "";
    BLUE      = "";
    WHITE     = "";

    BOLD      = "";
    DIM       = "";
    ITALIC    = "";
    UNDERLINE = "";
}

void printMetricSingle(
    const char* label,
    float usage
) {
    printf(
        "│ %-5s %s%s%5.1f%% %s%s             │\n",
        label,
        getColor(usage),
        BOLD,
        usage,
        getBars(usage),
        RESET
    );
}

void printLoadSingle(
    const char* labal,
    float usage
) {
    printf(
        "│ %-7s %.2f                    │\n",
        labal,
        usage
    );
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

void printTransferSingle(
    const char* label,
    float value
) {
    printf(
        "│ %-8s %-6.2f KB/s                 │\n",
        label,
        value
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

char *statuses[] = {
    "RUNNING",
    "SLEEPING",
    "ZOMBIE",
    "STOPPED",
    "DISK_SLEEP",
    "UNKNOWN"
};

void printProcessExtra(
    Process process,
    System system
) {
    printf("   └────── Status %s\n", statuses[process.status]);
    printf("    └────── Parent PID %d\n", process.parent_pid);
    printf("     └────── Threads %" PRIu64 "\n", process.threads);

    char human_readable_time[256];
    formatTimeHumanReadable(process.uptime, human_readable_time, 256);
    printf("      └────── Uptime %s\n", human_readable_time);
}

void render(
    CWArgs cwargs,
    System system,
    Metrics metrics 
) {
    renderSnapshot(cwargs, system, metrics);

    printf("Press 'd' to detach | Press 'q' to quit\n");
}

void renderSnapshot(
    CWArgs cwargs,
    System system,
    Metrics metrics 
) {
    size_t uptime_buffer_text_size = BUFFER_ONE_KB / 8;
    char uptime_buffer_text[uptime_buffer_text_size];
    formatTimeHumanReadable(system.uptime, uptime_buffer_text, uptime_buffer_text_size);
    if(cwargs.web) {
        printf("%s%sCherries Pulse%s ────── %d°C ───────── port :: %d ──────── ", BOLD, RED, RESET, system.temp / 1000, cwargs.port);
    } else {
        printf("%s%sCherries Pulse%s ───────────────── %d°C ─────────────────── ", BOLD, RED, RESET, system.temp / 1000);
    }

    printf("%-7s %s%-20s%s\n",
        "Uptime:",
        BOLD,
        uptime_buffer_text,
        RESET
    );

    if(cwargs.display.resources && cwargs.display.load) {
        printf("┌── RESOURCES ────────────────────────┐ ┌── SYSTEM LOAD ──────────────────┐\n");
        printMetric("CPU", metrics.cpuUsage, "1 min:", system.load.load1);
        printMetric("RAM", metrics.ramUsage, "5 min:", system.load.load5);
        printMetric("DISK", metrics.diskUsage, "15 min:", system.load.load15);
        printf("└─────────────────────────────────────┘ └─────────────────────────────────┘\n");
    } else if(cwargs.display.resources) {
        printf("┌── RESOURCES ────────────────────────┐\n");
        printMetricSingle("CPU", metrics.cpuUsage);
        printMetricSingle("RAM", metrics.ramUsage);
        printMetricSingle("DISK", metrics.diskUsage);
        printf("└─────────────────────────────────────┘\n");
    } else if(cwargs.display.load) {
        printf("┌── SYSTEM LOAD ──────────────────┐\n");
        printLoadSingle("1 min:", system.load.load1);
        printLoadSingle("5 min:", system.load.load5);
        printLoadSingle("15 min:", system.load.load15);
        printf("└─────────────────────────────────┘\n");
    }

    if(cwargs.display.network && cwargs.display.io) {
        printf("┌── NETWORK ──────────────────────────┐ ┌── DISK I/O ─────────────────────┐\n");
        printTransferRow("RX:", metrics.rx / 1024.0f, "READ:", metrics.read);
        printTransferRow("TX:", metrics.tx / 1024.0f, "WRITE:", metrics.write);
        printf("└─────────────────────────────────────┘ └─────────────────────────────────┘\n");
    } else if(cwargs.display.network) {
        printf("┌── NETWORK ───────────────────────────┐\n");
        printTransferSingle("RX:", metrics.rx / 1024.0f);
        printTransferSingle("TX:", metrics.tx / 1024.0f);
        printf("└──────────────────────────────────────┘\n");
    } else if(cwargs.display.io) {
        printf("┌── DISK I/O ──────────────────────────┐\n");
        printTransferSingle("READ:", metrics.read);
        printTransferSingle("WRITE:", metrics.write);
        printf("└──────────────────────────────────────┘\n");
    }

    if(cwargs.display.processes) {
        printf("┌── PROCESSES ────────────────────────────────────────────────────────────┐\n");
        printf("│ Running processes: %-52u │\n", system.process_count);
        for(unsigned i = 0; i < cwargs.processes; i++) {
            Process process = system.processes[i];
            printProcess(process, system);
        }
        printf("└─────────────────────────────────────────────────────────────────────────┘\n");
    }
}

void renderInfo(
    CWArgs cwargs,
    System system,
    Info info
) {
    printf(
        "%s%sCherries Pulse%s ───────────────────────────────────────────────────────────┐\n",
        BOLD,
        RED,
        RESET
    );

    printf("┌── INFO ─────────────────────────────────────────────────────────────────┐\n");
    printf(
        "│ %-15s %54s  │\n",
        "OS",
        info.os.name
    );

    printf(
        "│ %-15s %54s  │\n",
        "Architecture",
        info.kernel.machine
    );

    printf(
        "│ %-15s %54s  │\n",
        "Kernel",
        info.kernel.sysname
    );

    printf(
        "│ %-15s %54s  │\n",
        "Hostname",
        info.hostname
    );

    printf(
        "│ %-15s %51ld GB  │\n",
        "RAM",
        (system.memory.total) / 1024 / 1024
    );

    printf(
        "│ %-15s %54s  │\n",
        "CPU",
        info.cpu_model
    );

    printf(
        "│ %-15s %54d  │\n",
        "Cores",
        info.cores
    );

    printf(
        "│ %-15s %54s  │\n",
        "Desktop",
        info.desktop
    );

    printf(
        "│ %-15s %54s  │\n",
        "IPv4",
        info.ip.ipv4
    );

    printf(
        "│ %-15s %54s  │\n",
        "IPv6",
        info.ip.ipv6
    );

    printf(
        "│ %-15s %54s  │\n",
        "Session",
        info.session
    );

    printf("└─────────────────────────────────────────────────────────────────────────┘\n");
}
