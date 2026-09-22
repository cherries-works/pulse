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
#include <unistd.h>

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"
#include "daemon.h"
#include "log.h"
#include "app.h"
#include "config.h"

void stop() {
    _log(
        L_INFO,
        "Running stop() -> command stop"
    );


    char *home = getenv("HOME");
    if(home == NULL) {
        _log(
            L_ERROR,
            "No HOME environment variable"
        );
        return;
    }

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
    // _log(
    //     L_INFO,
    //     "Running help() -> command help"
    // );

    size_t version_length = strlen(PULSE_VERSION);
    size_t dashes = version_length < 42 ? 43 - version_length : 1;

    printf("%s%sCherries Pulse%s ", BOLD, RED, RESET);
    for(size_t i = 0; i < dashes; i++) printf("─");
    printf(" %s ──── \n", PULSE_VERSION);
    printf(" > %-20s %-20s\n", "monitor", "Monitors your device (default option).");
    printf("     %s%-20s %-20s%s\n", DIM, "--port [number]", "Determine the port where the website will be hosted (omits --web).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--web", "Hosts website (and API) on default port 8080.", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--sleep", "How many seconds the program sleeps before updating (TUI only).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--headless", "Runs program without TUI (currently only useful with --web).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--processes", "Amount of processes that are being monitored (max. 10).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--sort", "Sorts the processes between \"cpu\" and \"ram\".", RESET);
    printf(" > %-20s %-20s\n", "info", "Displays system information.");
    printf("     %s%-20s %-20s%s\n", DIM, "--json", "Prints the info of the system in JSON format.", RESET);
    printf(" > %-20s %-20s\n", "stop", "Stops all running processes by Pulse.");
    printf(" > %-20s %-20s\n", "help", "Prints this.");
    printf(" > %-20s %-20s\n", "top", "Prints top processes that are currently running.");
    printf("     %s%-20s %-20s%s\n", DIM, "--processes", "Amount of processes that get printed (max. 100).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--sort", "Sorts the processes between \"cpu\" and \"ram\".", RESET);
    printf(" > %-20s %-20s\n", "snapshot", "The current snapshot of the system.");
    printf("     %s%-20s %-20s%s\n", DIM, "--json", "Prints the info of the system in JSON format.", RESET);
    printf(" > %-20s %-20s\n", "prune", "Prunes either logs, history or both.");
    printf("     %s%-20s %-20s%s\n", DIM, "--keep", "Amount of files to be kept.", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--until", "The date up until when history/logs are kept. (YYYY-MM-DD)", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--prune", "What is suppose to be pruned (history/logs/all).", RESET);
    printf(" > %-20s %-20s\n", "config", "Configures ~/.cherries-works/pulse/config-toml file via nano.");
    printf(" > %-20s %-20s\n", "version", "Prints the current version.");
    printf("     %s%-20s %-20s%s\n", DIM, "--hash", "Prints the commit hash of the build.", RESET);
    printf("\n");
    // stop();
}

void top(Args args) {
    // _log(
    //     L_INFO,
    //     "Running top() -> command top"
    // );

    System system = getSystem(args);
    printf("%s%sCherries Pulse%s ───────────────────────────────────────────────────────────┐\n", BOLD, RED, RESET);
    printf("┌── PROCESSES ────────────────────────────────────────────────────────────┐\n");
    for(unsigned i = 0; i < args.processes; i++) {
        Process process = system.processes[i];
        printProcess(process, system);
    }
    printf("└─────────────────────────────────────────────────────────────────────────┘\n");
    // stop();
}

void info(Args args) {
    // _log(
    //     L_INFO,
    //     "Running info() -> command info"
    // );

    System system = getSystem(args);
    Info info = getInfo();
    if(args.json) {
        char snapshot_json[BUFFER_ONE_KB * 32];
        size_t snapshot_json_len = 0;
    
        snapshot_json_len += (size_t)snprintf(snapshot_json + snapshot_json_len, sizeof(snapshot_json) - snapshot_json_len,
            "{"
            "\"timestamp\":%ld,"
            "\"os\":\"%s\","
            "\"cores\":\"%d\","
            "\"cpu_model\":\"%s\","
            "\"desktop\":\"%s\","
            "\"hostname\":\"%s\","
            "\"kernel\":{\"machine\":\"%s\",\"release\":\"%s\",\"sysname\":\"%s\"},"
            "\"session\":\"%s\""
            "}",
            time(NULL),
            info.os,
            info.cores,
            info.cpu_model,
            info.desktop,
            info.hostname,
            info.kernel.machine,
            info.kernel.release,
            info.kernel.sysname,
            info.session
        );

        printf("%s", snapshot_json);
    } else {
        renderInfo(args, system, info);
    }

    // stop();
}

void monitor(Args args, Config config) {
    _log(
        L_INFO,
        "Running monitor() -> command monitor"
    );
    
    // clean up any leftover from a crash
    sem_unlink(CHERRIES_PULSE_READY_SEM);
    sem_t *ready_sem = sem_open(CHERRIES_PULSE_READY_SEM, O_CREAT | O_EXCL, 0600, 0);
    if (ready_sem == SEM_FAILED) {
        _log(L_ERROR, "Failed to create ready semaphore");
        exit(EXIT_FAILURE);
    }

    startDaemon(args, config);
    
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
    _log(
        L_INFO,
        "Running process() -> command process"
    );

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
    _log(
        L_INFO,
        "Running prune() -> command prune"
    );

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

void config() {
    _log(
        L_INFO,
        "Running config() -> command config"
    );

    char *home = getenv("HOME");
    if(home == NULL) {
        _log(
            L_ERROR,
            "No HOME environment variable"
        );
        return;
    }

    char path[BUFFER_ONE_KB];
    snprintf(path, BUFFER_ONE_KB, "%s/%s/config.toml", home, R_CHERRIES_FOLDER_PULSE);
    
    char command[BUFFER_ONE_KB];
    snprintf(command, BUFFER_ONE_KB, "nano %s", path);

    int result = system(command);
    if(result <= 0) return;

    stop();
}

void snapshot(Args args) {
    System system_snapshot = getSystem(args);
    sleep(1);
    System prev_system_snapshot = getSystem(args);
    Metrics metrics = getMetrics(system_snapshot, prev_system_snapshot);

    if(args.json) {
        char snapshot_json[BUFFER_ONE_KB * 32];
        size_t snapshot_json_len = 0;
    
        snapshot_json_len += (size_t)snprintf(snapshot_json + snapshot_json_len, sizeof(snapshot_json) - snapshot_json_len,
            "{"
            "\"timestamp\":%ld,"
            "\"metrics\":{\"cpuUsage\":%.2f,\"ramUsage\":%.2f,\"diskUsage\":%.2f,\"read\":%.2f,\"write\":%.2f,\"rx\":%.2f,\"tx\":%.2f},"
            "\"cpu\":{\"idle\":%ld,\"total\":%ld,\"processes\":%ld},"
            "\"disk\":{\"available\":%llu,\"total\":%llu,\"reads\":%llu,\"writes\":%llu},"
            "\"memory\":{\"available\":%ld,\"total\":%ld},"
            "\"network\":{\"rx\":%ld,\"tx\":%ld},"
            "\"load\":{\"load1\":%.2f,\"load5\":%.2f,\"load15\":%.2f},"
            "\"processes\":[",
            time(NULL),

            metrics.cpuUsage,
            metrics.ramUsage,
            metrics.diskUsage,
            metrics.read,
            metrics.write,
            metrics.rx,
            metrics.tx,
            
            system_snapshot.cpu.idle,
            system_snapshot.cpu.total,
            system_snapshot.cpu.processes,
    
            system_snapshot.disk.available,
            system_snapshot.disk.total,
            system_snapshot.disk.read,
            system_snapshot.disk.write,
    
            system_snapshot.memory.available,
            system_snapshot.memory.total,
    
            system_snapshot.network.rx,
            system_snapshot.network.tx,
    
            system_snapshot.load.load1,
            system_snapshot.load.load5,
            system_snapshot.load.load15
        );
    
        for (unsigned i = 0; i < system_snapshot.processes_count; i++) {
            if (i != 0) {
                snapshot_json_len += (size_t)snprintf(snapshot_json + snapshot_json_len, sizeof(snapshot_json) - snapshot_json_len, ",");
            }
    
            snapshot_json_len += (size_t)snprintf(snapshot_json + snapshot_json_len, sizeof(snapshot_json) - snapshot_json_len,
                "{\"pid\":%d,\"ram\":%ld,\"cpu\":%ld,\"name\":\"%s\"}",
                system_snapshot.processes[i].pid,
                system_snapshot.processes[i].ram,
                system_snapshot.processes[i].cpu,
                system_snapshot.processes[i].name
            );
        }
    
        snapshot_json_len += (size_t)snprintf(snapshot_json + snapshot_json_len, sizeof(snapshot_json) - snapshot_json_len,
            "],"
            "\"uptime\":%ld,"
            "\"temp\":%d"
            "}",
            system_snapshot.uptime,
            system_snapshot.temp
        );

        printf("%s", snapshot_json);
    } else {
        renderSnapshot(args, system_snapshot, metrics);
    }
}

void version(Args args) {
    if(args.hash) {
        printf("%s\n", PULSE_COMMIT);
    } else {
        printf("%s\n", PULSE_VERSION);
    }
}
