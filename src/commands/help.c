#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"
#include "daemon.h"
#include "log.h"
#include "app.h"
#include "setup.h"

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