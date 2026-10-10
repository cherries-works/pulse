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
#include <inttypes.h>

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"
#include "daemon.h"
#include "log.h"
#include "app.h"
#include "config.h"
#include "setup.h"

void handle(Args args, Config _config, struct CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Handler is parsing..."
    );

    Command cmd = args.command;

    switch (cmd) {
        case STOP:
            stop(cwinfo);
            break;

        case HELP:
            help(cwinfo);
            break;
        
        case MONITOR:
            monitor(args, _config, cwinfo);
            break;
        
        case INFO:
            info(args, cwinfo);
            break;
        
        case TOP:
            top(args, cwinfo);
            break;
        
        case PROCESS:
            process(args, cwinfo);
            break;
        
        case PRUNE:
            prune(args, cwinfo);
            break;

        case CONFIG:
            config(args, cwinfo);
            break;
        
        case SNAPSHOT:
            snapshot(args, cwinfo);
            break;

        case VERSION:
            version(args);
            break;
        
        default:
            break;
    }

    // cannot log, if we stopped the logger
    if(cmd == STOP) return; 

    _log(
        cwinfo,
        L_INFO,
        "Finished running command"
    );
}

void stop(struct CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running stop() -> command stop"
    );

    char file_path[BUFFER_ONE_KB];
    char path_dir[BUFFER_ONE_KB / 2];
    snprintf(
        path_dir,
        BUFFER_ONE_KB / 2,
        "%s/state",
        cwinfo.pulse
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

    printLogFile(cwinfo);

    closedir(dir);
}

void help(struct CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running help() -> command help"
    );

    size_t version_length = strlen(PULSE_VERSION);
    size_t dashes = version_length < 42 ? 43 - version_length : 1;

    printf("%s%sCherries Pulse%s ", BOLD, RED, RESET);
    for(size_t i = 0; i < dashes; i++) printf("─");
    printf(" %s ──── \n", PULSE_VERSION);
    printf(" > %-20s %-20s\n", "monitor", "Monitors your device (default option).");
    printf("     %s%-20s %-20s%s\n", DIM, "--port [number]", "Determine the port where the website will be hosted (omits --web).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--web", "Hosts website (and API) on default port 8383.", RESET);
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
    printf(" > %-20s %-20s\n", "process", "Print details about a specific process.");
    printf("     %s%-20s %-20s%s\n", DIM, "--process", "The process PID [required].", RESET);
    printf(" > %-20s %-20s\n", "prune", "Prunes either logs, history or both.");
    printf("     %s%-20s %-20s%s\n", DIM, "--keep", "Amount of files to be kept.", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--until", "The date up until when history/logs are kept. (YYYY-MM-DD)", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--prune", "What is suppose to be pruned (history/logs/all).", RESET);
    printf(" > %-20s %-20s\n", "config", "Configures ~/.cherries-works/pulse/config.toml file via nano.");
    printf("     %s%-20s %-20s%s\n", DIM, "--reset", "Resets the config file to the pulse standard config file (destructive).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--current", "Prints the current configuration.", RESET);
    printf(" > %-20s %-20s\n", "version", "Prints the current version.");
    printf("     %s%-20s %-20s%s\n", DIM, "--hash", "Prints the commit hash of the build.", RESET);
    printf("\n");
    stop(cwinfo);
}

void top(Args args, struct CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running top() -> command top"
    );

    System system = getSystem(cwinfo, args);
    printf("%s%sCherries Pulse%s ───────────────────────────────────────────────────────────┐\n", BOLD, RED, RESET);
    printf("┌── PROCESSES ────────────────────────────────────────────────────────────┐\n");
    for(unsigned i = 0; i < args.processes; i++) {
        Process process = system.processes[i];
        printProcess(process, system);
    }
    printf("└─────────────────────────────────────────────────────────────────────────┘\n");
    stop(cwinfo);
}

void info(Args args, struct CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running info() -> command info"
    );

    System system = getSystem(cwinfo, args);
    Info info = getInfo();
    if(args.json) {
        char snapshot_json[BUFFER_ONE_KB * 32];
        size_t snapshot_json_len = 0;
    
        snapshot_json_len += (size_t)snprintf(snapshot_json + snapshot_json_len, sizeof(snapshot_json) - snapshot_json_len,
            "{"
            "\"timestamp\":%" PRIu64 ","
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

    stop(cwinfo);
}

void monitor(Args args, Config config, struct CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running monitor() -> command monitor"
    );
    
    // clean up any leftover from a crash
    sem_unlink(CHERRIES_PULSE_READY_SEM);
    sem_t *ready_sem = sem_open(CHERRIES_PULSE_READY_SEM, O_CREAT | O_EXCL, 0600, 0);
    if (ready_sem == SEM_FAILED) {
        _log(cwinfo, L_ERROR, "Failed to create ready semaphore");
        return;
    }

    startDaemon(args, config, cwinfo);
    
    // wait until daemon is ready
    sem_wait(ready_sem);
    sem_post(ready_sem);
    sem_close(ready_sem);
    sem_unlink(CHERRIES_PULSE_READY_SEM);

    if(args.headless && !args.web) return;

    if(args.web) {
        startWebsite(args, cwinfo);
    }

    if(!args.headless) {
        startRender(args, cwinfo);
    }

    return;
}

void process(Args args, struct CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running process() -> command process"
    );

    System system = getSystem(cwinfo, args);

    Process process = {
        .cpu = 0,
        .pid = 0,
        .ram = 0,
        .name = ""
    };

    getProcess(cwinfo, &process, args.process);

    printf("%s%sCherries Pulse%s ───────────────────────────────────────────────────────────┐\n", BOLD, RED, RESET);
    printf("┌── PROCESS (%-6d) ─────────────────────────────────────────────────────┐\n", args.process);
    printProcess(process, system);
    printf("└─────────────────────────────────────────────────────────────────────────┘\n");
    printProcessExtra(process, system);
    printf("───────────────────────────────────────────────────────────────────────────\n");
    stop(cwinfo);
}

void prune(Args args, struct CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running prune() -> command prune"
    );

    Prune prune = args.prune;
    unsigned keep = args.keep;
    uint64_t until = unformatTime(args.until);

    size_t path_size = BUFFER_ONE_KB;
    if(prune == HISTORY || prune == ALL) {
        char path[path_size];
        snprintf(path, path_size, "%s/history", cwinfo.pulse);

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

            snprintf(entry_path, path_size, "%s/history/%s", cwinfo.pulse, entry_name);

            uint64_t _entry_time = unformatTime(entry_name);
            if(_entry_time < until) {
                cleanDir(entry_path);
                path_entries_deleted++;
            }
        }

        closedir(dir);
    }

    if(prune == LOGS || prune == ALL) {
        char path[path_size];
        snprintf(path, path_size, "%s/logs", cwinfo.pulse);

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

            snprintf(entry_path, path_size, "%s/logs/%s", cwinfo.pulse, entry_name);
            uint64_t _entry_time = unformatTime(entry_name);

            if(_entry_time < until) {
                remove(entry_path);
                path_entries_deleted++;
            }
        }

        closedir(dir);
    }

    stop(cwinfo);
}

void config(Args args, struct CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running config() -> command config"
    );

    char path[BUFFER_ONE_KB];
    snprintf(path, BUFFER_ONE_KB, "%s/config.toml", cwinfo.pulse);
   
    // enum to string conversion
    const char* operatorToString(Operator op) {
        switch (op) {
            case G:  return ">";
            case E:  return "==";
            case L:  return "<";
            case GE: return ">=";
            case LE: return "<=";
            case NE: return "!=";
            default: return ">";
        }
    }

    if(args.current) {
        Config config = parseToml(cwinfo);
        printf("Alerts\n");
        printf("CPU:  %-2s %d%% for %ds\n", operatorToString(config.alerts.CPU.op), config.alerts.CPU.threshold, config.alerts.CPU.duration);
        printf("RAM:  %-2s %d%% for %ds\n", operatorToString(config.alerts.RAM.op), config.alerts.RAM.threshold, config.alerts.RAM.duration);
        printf("Disk: %-2s %d%% for %ds\n", operatorToString(config.alerts.Disk.op), config.alerts.Disk.threshold, config.alerts.Disk.duration);

        printf("\nNotifications\n");
        printf("Desktop:  %s\n", config.desktopNotify.enabled ? "enabled" : "disabled");
        printf("Discord:  %s\n", config.discordNotify.enabled ? "enabled" : "disabled");
        printf("Command:  %s\n", config.commandNotify.enabled ? "enabled" : "disabled");
        return;
    }

    if(args.reset) {
        remove(path);
        int fd = creat(path, 0644);
        ssize_t result = write(fd, CHERRIES_DEFAULT_TOML, strlen(CHERRIES_DEFAULT_TOML));
        if(result <= 0) {
            _log(cwinfo, L_ERROR, "Failed to reset config file.");
        }
        return;
    }
    char command[BUFFER_ONE_KB];
    snprintf(command, BUFFER_ONE_KB, "nano %s", path);

    int result = system(command);
    if(result <= 0) return;

    stop(cwinfo);
}

void snapshot(Args args, struct CWInfo cwinfo) {
    System system_snapshot = getSystem(cwinfo, args);
    sleep(1);
    System prev_system_snapshot = getSystem(cwinfo, args);
    Metrics metrics = getMetrics(system_snapshot, prev_system_snapshot);

    if(args.json) {
        char snapshot_json[BUFFER_ONE_KB * 32];
        size_t snapshot_json_len = 0;
    
        snapshot_json_len += (size_t)snprintf(snapshot_json + snapshot_json_len, sizeof(snapshot_json) - snapshot_json_len,
            "{"
            "\"timestamp\":%" PRIu64 ","
            "\"metrics\":{\"cpuUsage\":%.2f,\"ramUsage\":%.2f,\"diskUsage\":%.2f,\"read\":%.2f,\"write\":%.2f,\"rx\":%.2f,\"tx\":%.2f},"
            "\"cpu\":{\"idle\":%" PRIu64 ",\"total\":%" PRIu64 ",\"processes\":%" PRIu64 "},"
            "\"disk\":{\"available\":%" PRIu64 ",\"total\":%" PRIu64 ",\"reads\":%" PRIu64 ",\"writes\":%" PRIu64 "},"
            "\"memory\":{\"available\":%" PRIu64 ",\"total\":%" PRIu64 "},"
            "\"network\":{\"rx\":%" PRIu64 ",\"tx\":%" PRIu64 "},"
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
                "{\"pid\":%d,\"ram\":%" PRIu64 ",\"cpu\":%" PRIu64 ",\"name\":\"%s\"}",
                system_snapshot.processes[i].pid,
                system_snapshot.processes[i].ram,
                system_snapshot.processes[i].cpu,
                system_snapshot.processes[i].name
            );
        }
    
        snapshot_json_len += (size_t)snprintf(snapshot_json + snapshot_json_len, sizeof(snapshot_json) - snapshot_json_len,
            "],"
            "\"uptime\":%" PRIu64 ","
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
