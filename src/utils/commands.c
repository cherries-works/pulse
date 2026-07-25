#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <wait.h>

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"
#include "daemon.h"
#include "log.h"
#include "app.h"

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

void help() {
    printf("%s%sCherries Pulse%s ───────────────────────────────────── v0.3.1 ──── \n", BOLD, RED, RESET);
    printf(" > %-20s %-20s\n", "monitor", "Monitors your device (default option).");
    printf("     %s%-20s %-20s%s\n", DIM, "--port [number]", "Determine the port where the website will be hosted (omits --web).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--web", "Hosts website (and API) on default port 8080.", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--sleep", "How many seconds the program sleeps before updating (TUI only).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--headless", "Runs program without TUI (currently only useful with --web).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--processes", "Amount of processes that are being monitored (max. 10).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--sort", "Sorts the processes between \"cpu\" and \"ram\".", RESET);
    printf(" > %-20s %-20s\n", "stop", "Stops all running processes by Pulse.");
    printf(" > %-20s %-20s\n", "help", "Prints this.");
    printf(" > %-20s %-20s\n", "info", "Displays system information.");
    printf(" > %-20s %-20s\n", "top", "Prints top processes that are currently running.");
    printf("     %s%-20s %-20s%s\n", DIM, "--processes", "Amount of processes that get printed (max. 100).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--sort", "Sorts the processes between \"cpu\" and \"ram\".", RESET);
    printf(" > %-20s %-20s\n", "prune", "Prunes either logs, history or both.");
    printf("     %s%-20s %-20s%s\n", DIM, "--keep", "Amount of files to be kept.", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--until", "The date up until when history/logs are kept. (YYYY-MM-DD)", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--prune", "What is suppose to be pruned (history/logs/all).", RESET);
    printf("\n");
    stop();
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
    stop();
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
    stop();
}

void monitor(Args args) {
    // clean up any leftover from a crash
    sem_unlink(CHERRIES_PULSE_READY_SEM);
    sem_t *ready_sem = sem_open(CHERRIES_PULSE_READY_SEM, O_CREAT | O_EXCL, 0600, 0);
    if (ready_sem == SEM_FAILED) {
        _log(L_ERROR, "Failed to create ready semaphore");
        exit(EXIT_FAILURE);
    }

    startDaemon(args);
    
    // wait until daemon is ready
    sem_wait(ready_sem);
    sem_post(ready_sem);
    sem_close(ready_sem);
    sem_unlink(CHERRIES_PULSE_READY_SEM);

    if(args.headless && !args.web) return;

    if(args.web) {
        startWebsite(args);
    }

    if(!args.headless) {
        startRender(args);
    }

    return;
}

void process(Args args) {
    System system = getSystem(args);

    Process process = {
        .cpu = 0,
        .pid = 0,
        .ram = 0,
        .name = ""
    };

    getProcess(&process, args.process);

    printf("%s%sCherries Pulse%s ───────────────────────────────────────────────────────────┐\n", BOLD, RED, RESET);
    printf("┌── PROCESS (%-6d) ─────────────────────────────────────────────────────┐\n", args.process);
    printProcess(process, system);
    printf("└─────────────────────────────────────────────────────────────────────────┘\n");
    printProcessExtra(process, system);
    printf("───────────────────────────────────────────────────────────────────────────\n");
    stop();
}

void prune(Args args) {
    Prune prune = args.prune;
    unsigned keep = args.keep;
    long unsigned until = unformatTime(args.until);

    char *home = getenv("HOME");
    if(home == NULL) {
        _log(L_ERROR, "No HOME environment variable");
        return;
    }

    size_t path_size = BUFFER_ONE_KB;
    if(prune == HISTORY || prune == ALL) {
        char path[path_size];
        snprintf(path, path_size, "%s/%s/history", home, R_CHERRIES_FOLDER_PULSE);

        unsigned path_entries_count = countDir(path);
        unsigned path_entries_deleted = 0;

        char entry_path[path_size];

        struct dirent *entry;
        DIR *dir = opendir(path);

        while((entry = readdir(dir)) != NULL) {
            if(path_entries_count - path_entries_deleted <= keep) break;

            char *entry_name = entry->d_name;
            if(strcmp(entry_name, ".") == 0) continue;
            if(strcmp(entry_name, "..") == 0) continue;

            snprintf(entry_path, path_size, "%s/%s/history/%s", home, R_CHERRIES_FOLDER_PULSE, entry_name);

            long unsigned _entry_time = unformatTime(entry_name);
            if(_entry_time < until) {
                cleanDir(entry_path);
                path_entries_deleted++;
            }
        }

        closedir(dir);
    }

    if(prune == LOGS || prune == ALL) {
        char path[path_size];
        snprintf(path, path_size, "%s/%s/logs", home, R_CHERRIES_FOLDER_PULSE);

        unsigned path_entries_count = countDir(path);
        unsigned path_entries_deleted = 0;

        char entry_path[path_size];

        struct dirent *entry;
        DIR *dir = opendir(path);
        
        while((entry = readdir(dir)) != NULL) {
            if(path_entries_count - path_entries_deleted <= keep) break;

            char *entry_name = entry->d_name;
            if(strcmp(entry_name, ".") == 0) continue;
            if(strcmp(entry_name, "..") == 0) continue;

            snprintf(entry_path, path_size, "%s/%s/logs/%s", home, R_CHERRIES_FOLDER_PULSE, entry_name);
            long unsigned _entry_time = unformatTime(entry_name);

            if(_entry_time < until) {
                remove(entry_path);
                path_entries_deleted++;
            }
        }

        closedir(dir);
    }

    stop();
}
