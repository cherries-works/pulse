#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#include "parse.h"
#include "render.h"
#include "utils.h"

void help() {
    printf("%s%sCherries Pulse%s ───────────────────────────────────── v0.2.0 ──── \n\n", BOLD, RED, RESET);
    printf("     %-20s %-20s\n", "--port [number]", "Determine the port where the website will be hosted (omits --web).");
    printf("     %-20s %-20s\n", "--web", "Hosts website (and API) on default port 8080.");
    printf("     %-20s %-20s\n", "--sleep", "How many seconds the program sleeps before updating (TUI only).");
    printf("     %-20s %-20s\n", "--headless", "Runs program without TUI (currently only useful with --web).");
    printf("     %-20s %-20s\n", "--stop", "Stops all of the processes that Pulse currently runs.");
    printf("     %-20s %-20s\n", "--processes", "Amount of processes that are being monitored.");
    printf("     %-20s %-20s\n", "--sort", "Sorts the processes between \"cpu\" and \"ram\".");
    printf("     %-20s %-20s\n\n", "--help", "Prints this.");
}

void stop() {
    char *home = getenv("HOME");
    if(home == NULL) return;

    char file_path[BUFFER_ONE_KB];
    char path_dir[BUFFER_ONE_KB / 2];
    snprintf(
        path_dir,
        BUFFER_ONE_KB / 2,
        "%s/%s/state",
        home, 
        R_CHERRIES_FOLDER_PULSE
    );

    DIR *dir = opendir(path_dir);
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL) {
        char *name = entry->d_name;
        if(strcmp(name, ".") == 0) continue;
        if(strcmp(name, "..") == 0) continue;
        snprintf(file_path, BUFFER_ONE_KB, "%s/%s", path_dir, name);

        char *d = strchr(name, '.');
        if(d == NULL) continue;
        name = d + 1;
        int pid = atoi(name);
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
        
        remove(file_path);
    }

    snprintf(file_path, BUFFER_ONE_KB, "%s/%s/state/log", home, R_CHERRIES_FOLDER_PULSE);
    remove(file_path);

    closedir(dir);
}

void top(Args args) {
    System system = getSystem(args);
    printf("%s%sCherries Pulse%s ───────────────────────────────────────────────────────────┐\n", BOLD, RED, RESET);
    printf("┌── PROCESSES ────────────────────────────────────────────────────────────┐\n");
    for(unsigned i = 0; i < args.processes; i++) {
        Process process = system.processes[i];
        printProcess(process, system);
    }
    printf("└─────────────────────────────────────────────────────────────────────────┘\n");
}

void info(Args args) {
    System system = getSystem(args);
    Info info = getInfo();
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
        info.os
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
        "Session",
        info.session
    );

    printf("└─────────────────────────────────────────────────────────────────────────┘\n");
}