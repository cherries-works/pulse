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
#include "app.h"
#include "setup.h"

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
    
        for (uint16_t i = 0; i < system_snapshot.processes_count; i++) {
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
