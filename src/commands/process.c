#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"
#include "daemon.h"
#include "log.h"
#include "app.h"
#include "setup.h"

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
