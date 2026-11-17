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

        for(uint8_t i = 0; i < cwargs.processes; i++) {
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
    for(uint8_t i = 0; i < cwargs.processes; i++) {
        Process process = system.processes[i];
        printProcess(process, system);
    }
    printf("└─────────────────────────────────────────────────────────────────────────┘\n");
}
