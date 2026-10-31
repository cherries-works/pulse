#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
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

void handle(CWArgs cwargs, CWConfig _cwconfig, CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Handler is parsing..."
    );

    if (cwargs.no_color) {
        disableColors();
    }

    Command cmd = cwargs.command;

    switch (cmd) {
        case STOP:
            stop(cwinfo);
            break;

        case HELP:
            help(cwinfo);
            break;
        
        case MONITOR:
            monitor(cwargs, _cwconfig, cwinfo);
            break;
        
        case INFO:
            info(cwargs, cwinfo);
            break;
        
        case TOP:
            top(cwargs, cwinfo);
            break;
        
        case PROCESS:
            process(cwargs, cwinfo);
            break;
        
        case PRUNE:
            prune(cwargs, cwinfo);
            break;

        case CONFIG:
            config(cwargs, cwinfo);
            break;
        
        case SNAPSHOT:
            snapshot(cwargs, cwinfo);
            break;

        case VERSION:
            version(cwargs);
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

void stop(CWInfo cwinfo) {
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

void help(CWInfo cwinfo) {
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
    printf("     %s%-20s %-20s%s\n", DIM, "--style", "1/2/3, styling choices.", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--history-snapshot-interval", "The interval defines how many sleeps (-sleep) have to pass for a history snapshot to be taken (default: 2).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--sleep", "How many seconds the program sleeps before updating (TUI only).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--headless", "Runs program without TUI (currently only useful with --web).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--processes", "Amount of processes that are being monitored (max. 10).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--sort", "Sorts the processes between \"cpu\" and \"ram\".", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--no-color or -nc", "removes colors so everything is printed without ANSI styling.", RESET);
    printf(" > %-20s %-20s\n", "info", "Displays system information.");
    printf("     %s%-20s %-20s%s\n", DIM, "--json", "Prints the info of the system in JSON format.", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--no-color or -nc", "removes colors so everything is printed without ANSI styling.", RESET);
    printf(" > %-20s %-20s\n", "stop", "Stops all running processes by Pulse.");
    printf(" > %-20s %-20s\n", "help", "Prints this.");
    printf("     %s%-20s %-20s%s\n", DIM, "--no-color or -nc", "removes colors so everything is printed without ANSI styling.", RESET);
    printf(" > %-20s %-20s\n", "top", "Prints top processes that are currently running.");
    printf("     %s%-20s %-20s%s\n", DIM, "--processes", "Amount of processes that get printed (max. 100).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--sort", "Sorts the processes between \"cpu\" and \"ram\".", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--json", "Prints the top processes in JSON format.", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--no-color or -nc", "removes colors so everything is printed without ANSI styling.", RESET);
    printf(" > %-20s %-20s\n", "snapshot", "The current snapshot of the system.");
    printf("     %s%-20s %-20s%s\n", DIM, "--json", "Prints the info of the system in JSON format.", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--no-color or -nc", "removes colors so everything is printed without ANSI styling.", RESET);
    printf(" > %-20s %-20s\n", "process", "Print details about a specific process.");
    printf("     %s%-20s %-20s%s\n", DIM, "--process", "The process PID [required].", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--json", "Prints the details of the process in JSON format.", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--no-color or -nc", "removes colors so everything is printed without ANSI styling.", RESET);
    printf(" > %-20s %-20s\n", "prune", "Prunes either logs, history or both.");
    printf("     %s%-20s %-20s%s\n", DIM, "--keep", "Amount of files to be kept.", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--until", "The date up until when history/logs are kept. (YYYY-MM-DD)", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--prune", "What is suppose to be pruned (history/logs/all).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--json", "Prints the amount of deleted history/logs entries in JSON format.", RESET);
    printf(" > %-20s %-20s\n", "cwconfig", "Configures ~/.cherries-works/pulse/cwconfig.toml file via nano.");
    printf("     %s%-20s %-20s%s\n", DIM, "--reset", "Resets the cwconfig file to the pulse standard cwconfig file (destructive).", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--current", "Prints the current configuration.", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--json", "Prints the current configuration in JSON format (with --current).", RESET);
    printf(" > %-20s %-20s\n", "version", "Prints the current version.");
    printf("     %s%-20s %-20s%s\n", DIM, "--hash", "Prints the commit hash of the build.", RESET);
    printf("     %s%-20s %-20s%s\n", DIM, "--json", "Prints the version and commit hash in JSON format.", RESET);
    printf("\n");
}

void top(CWArgs cwargs, CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running top() -> command top"
    );

    System system = getSystem(cwinfo, cwargs);

    if(cwargs.json) {
        char top_json[BUFFER_ONE_KB * 32];
        size_t top_json_len = 0;

        top_json_len += (size_t)snprintf(top_json + top_json_len, sizeof(top_json) - top_json_len,
            "{"
            "\"timestamp\":%" PRIu64 ","
            "\"processes\":[",
            time(NULL)
        );

        for(unsigned i = 0; i < cwargs.processes; i++) {
            if(i != 0) {
                top_json_len += (size_t)snprintf(top_json + top_json_len, sizeof(top_json) - top_json_len, ",");
            }

            char name[sizeof(system.processes[i].name) * 6];
            escapeJson(system.processes[i].name, name, sizeof(name));

            top_json_len += (size_t)snprintf(top_json + top_json_len, sizeof(top_json) - top_json_len,
                "{\"pid\":%d,\"ram\":%" PRIu64 ",\"cpu\":%" PRIu64 ",\"name\":\"%s\"}",
                system.processes[i].pid,
                system.processes[i].ram,
                system.processes[i].cpu,
                name
            );
        }

        top_json_len += (size_t)snprintf(top_json + top_json_len, sizeof(top_json) - top_json_len, "]}");

        printf("%s\n", top_json);
        return;
    }

    printf("%s%sCherries Pulse%s ───────────────────────────────────────────────────────────┐\n", BOLD, RED, RESET);
    printf("┌── PROCESSES ────────────────────────────────────────────────────────────┐\n");
    for(unsigned i = 0; i < cwargs.processes; i++) {
        Process process = system.processes[i];
        printProcess(process, system);
    }
    printf("└─────────────────────────────────────────────────────────────────────────┘\n");
}

void info(CWArgs cwargs, CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running info() -> command info"
    );

    System system = getSystem(cwinfo, cwargs);
    Info info = getInfo();
    if(cwargs.json) {
        char snapshot_json[BUFFER_ONE_KB * 32];
        size_t snapshot_json_len = 0;
    
        snapshot_json_len += (size_t)snprintf(snapshot_json + snapshot_json_len, sizeof(snapshot_json) - snapshot_json_len,
            "{"
            "\"timestamp\":%" PRIu64 ","
            "\"os\":{\"name\":\"%s\",\"type\":\"%s\"},"
            "\"cores\":\"%d\","
            "\"cpu_model\":\"%s\","
            "\"desktop\":\"%s\","
            "\"hostname\":\"%s\","
            "\"kernel\":{\"machine\":\"%s\",\"release\":\"%s\",\"sysname\":\"%s\"},"
            "\"session\":\"%s\""
            "}",
            time(NULL),
            info.os.name,
            OS_TYPE_SETTINGS[info.os.type],
            info.cores,
            info.cpu_model,
            info.desktop,
            info.hostname,
            info.kernel.machine,
            info.kernel.release,
            info.kernel.sysname,
            info.session
        );

        printf("%s\n", snapshot_json);
    } else {
        renderInfo(cwargs, system, info);
    }
}

void monitor(CWArgs cwargs, CWConfig cwconfig, CWInfo cwinfo) {
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

    startDaemon(cwargs, cwconfig, cwinfo);

    // wait until daemon is ready
    sem_wait(ready_sem);
    sem_post(ready_sem);
    sem_close(ready_sem);
    sem_unlink(CHERRIES_PULSE_READY_SEM);

    if(cwargs.headless && !cwargs.web) return;

    if(cwargs.web) {
        startWebsite(cwargs, cwinfo);
    }

    if(!cwargs.headless) {
        startRender(cwargs, cwinfo);
    }
}

void process(CWArgs cwargs, CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running process() -> command process"
    );

    System system = getSystem(cwinfo, cwargs);

    Process process = {
        .cpu = 0,
        .pid = 0,
        .ram = 0,
        .name = ""
    };

    if(cwargs.pid != -1) {
        int p = getProcess(cwinfo, &process, cwargs.pid);
        if(p == -1) return;
    } else {
        pid_t _pid = getPidByName(cwargs.name);
        if(_pid == -1) {
            _log(cwinfo, L_ERROR, "Invalid, or no name passed.");

            printf("Invalid, or no name passed.\n--name (name)\n\n");
            return;
        }

        int p = getProcess(cwinfo, &process, _pid);
        if(p == -1) return;
    }

    if(cwargs.json) {
        char name[sizeof(process.name) * 6];
        escapeJson(process.name, name, sizeof(name));

        printf(
            "{"
            "\"timestamp\":%" PRIu64 ","
            "\"pid\":%d,"
            "\"parent_pid\":%d,"
            "\"name\":\"%s\","
            "\"status\":\"%s\","
            "\"threads\":%" PRIu64 ","
            "\"uptime\":%" PRIu64 ","
            "\"ram\":%" PRIu64 ","
            "\"cpu\":%" PRIu64
            "}\n",
            time(NULL),
            process.pid,
            process.parent_pid,
            name,
            statuses[process.status],
            process.threads,
            process.uptime,
            process.ram,
            process.cpu
        );
        return;
    }

    printf("%s%sCherries Pulse%s ───────────────────────────────────────────────────────────┐\n", BOLD, RED, RESET);
    printf("┌── PROCESS (%-6d) ─────────────────────────────────────────────────────┐\n", process.pid);
    printProcess(process, system);
    printf("└─────────────────────────────────────────────────────────────────────────┘\n");
    printProcessExtra(process, system);
    printf("───────────────────────────────────────────────────────────────────────────\n");
}

void prune(CWArgs cwargs, CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running prune() -> command prune"
    );

    Prune prune = cwargs.prune;
    unsigned keep = cwargs.keep;
    uint64_t until = unformatTime(cwargs.until);

    size_t path_size = BUFFER_ONE_KB;
    unsigned history_deleted = 0;
    unsigned logs_deleted = 0;

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
        history_deleted = path_entries_deleted;
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
        logs_deleted = path_entries_deleted;
    }

    if(cwargs.json) {
        printf(
            "{"
            "\"timestamp\":%" PRIu64 ","
            "\"history\":%u,"
            "\"logs\":%u"
            "}\n",
            time(NULL),
            history_deleted,
            logs_deleted
        );
    }
}

void config(CWArgs cwargs, CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running cwconfig() -> command cwconfig"
    );

    char path[BUFFER_ONE_KB];
    snprintf(path, BUFFER_ONE_KB, "%s/cwconfig.toml", cwinfo.pulse);
   
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

    if(cwargs.current) {
        CWConfig _cwconfig = parseToml(cwinfo);

        if(cwargs.json) {
            printf(
                "{"
                "\"alerts\":{"
                "\"cpu\":{\"op\":\"%s\",\"threshold\":%d,\"duration\":%d},"
                "\"ram\":{\"op\":\"%s\",\"threshold\":%d,\"duration\":%d},"
                "\"disk\":{\"op\":\"%s\",\"threshold\":%d,\"duration\":%d}"
                "},"
                "\"notifications\":{\"desktop\":%s,\"discord\":%s,\"command\":%s}"
                "}\n",
                operatorToString(_cwconfig.alerts.CPU.op), _cwconfig.alerts.CPU.threshold, _cwconfig.alerts.CPU.duration,
                operatorToString(_cwconfig.alerts.RAM.op), _cwconfig.alerts.RAM.threshold, _cwconfig.alerts.RAM.duration,
                operatorToString(_cwconfig.alerts.Disk.op), _cwconfig.alerts.Disk.threshold, _cwconfig.alerts.Disk.duration,
                _cwconfig.desktopNotify.enabled ? "true" : "false",
                _cwconfig.discordNotify.enabled ? "true" : "false",
                _cwconfig.commandNotify.enabled ? "true" : "false"
            );
            return;
        }

        printf("Alerts\n");
        printf("CPU:  %-2s %d%% for %ds\n", operatorToString(_cwconfig.alerts.CPU.op), _cwconfig.alerts.CPU.threshold, _cwconfig.alerts.CPU.duration);
        printf("RAM:  %-2s %d%% for %ds\n", operatorToString(_cwconfig.alerts.RAM.op), _cwconfig.alerts.RAM.threshold, _cwconfig.alerts.RAM.duration);
        printf("Disk: %-2s %d%% for %ds\n", operatorToString(_cwconfig.alerts.Disk.op), _cwconfig.alerts.Disk.threshold, _cwconfig.alerts.Disk.duration);

        printf("\nNotifications\n");
        printf("Desktop:  %s\n", _cwconfig.desktopNotify.enabled ? "enabled" : "disabled");
        printf("Discord:  %s\n", _cwconfig.discordNotify.enabled ? "enabled" : "disabled");
        printf("Command:  %s\n", _cwconfig.commandNotify.enabled ? "enabled" : "disabled");
        return;
    }

    if(cwargs.reset) {
        remove(path);
        int fd = creat(path, 0644);
        ssize_t result = write(fd, CHERRIES_DEFAULT_TOML, strlen(CHERRIES_DEFAULT_TOML));
        if(result <= 0) {
            _log(cwinfo, L_ERROR, "Failed to reset cwconfig file.");
        }
        return;
    }
    char command[BUFFER_ONE_KB];
    snprintf(command, BUFFER_ONE_KB, "nano %s", path);

    int result = system(command);
    if(result <= 0) return;
}

void snapshot(CWArgs cwargs, CWInfo cwinfo) {
    System system_snapshot = getSystem(cwinfo, cwargs);
    sleep(1);
    System prev_system_snapshot = getSystem(cwinfo, cwargs);
    Metrics metrics = getMetrics(system_snapshot, prev_system_snapshot);

    if(cwargs.json) {
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
    
            char name[sizeof(system_snapshot.processes[i].name) * 6];
            escapeJson(system_snapshot.processes[i].name, name, sizeof(name));

            snapshot_json_len += (size_t)snprintf(snapshot_json + snapshot_json_len, sizeof(snapshot_json) - snapshot_json_len,
                "{\"pid\":%d,\"ram\":%" PRIu64 ",\"cpu\":%" PRIu64 ",\"name\":\"%s\"}",
                system_snapshot.processes[i].pid,
                system_snapshot.processes[i].ram,
                system_snapshot.processes[i].cpu,
                name
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

        printf("%s\n", snapshot_json);
    } else {
        renderSnapshot(cwargs, system_snapshot, metrics);
    }
}

void version(CWArgs cwargs) {
    if(cwargs.json) {
        printf("{\"version\":\"%s\",\"commit\":\"%s\"}\n", PULSE_VERSION, PULSE_COMMIT);
    } else if(cwargs.hash) {
        printf("%s\n", PULSE_COMMIT);
    } else {
        printf("%s\n", PULSE_VERSION);
    }
}
