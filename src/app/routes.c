#include <stdio.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <inttypes.h>

#include "http.h"
#include "daemon.h"
#include "log.h"
#include "parse.h"
#include "setup.h"

#define STATIC_ROUTE(fnName, filePath) \
void fnName(int socket, char *response, size_t response_size, struct CWInfo cwinfo) { \
    routeStatic(socket, filePath, response, response_size); \
}

#define JSON_ROUTE(fnName, body) \
void fnName(int socket, char *response, size_t response_size, struct CWInfo cwinfo) body

STATIC_ROUTE(indexHtml, "./src/app/static/index.html");
STATIC_ROUTE(indexStyle, "./src/app/static/css/style.css");
STATIC_ROUTE(indexJs, "./src/app/static/js/script.js");
STATIC_ROUTE(indexCwCharts, "./src/app/static/js/cw.charts.js");
STATIC_ROUTE(indexFavicon, "./src/app/static/assets/favicon.png");

JSON_ROUTE(indexMetrics, {
    System snapshot;
    readDaemonS(cwinfo, &snapshot);

    Info info = getInfo();

    char json[BUFFER_ONE_KB * 32];
    size_t json_len = 0;

    json_len += (size_t)snprintf(json + json_len, sizeof(json) - json_len,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json; charset=UTF-8\r\n\r\n"
        "{"
        "\"error\":null,"
        "\"success\":true,"
        "\"timestamp\":%" PRIu64 ","
        "\"cpu\":{\"idle\":%" PRIu64 ",\"total\":%" PRIu64 ",\"processes\":%" PRIu64 "},"
        "\"process_count\":%u,"
        "\"disk\":{\"available\":%" PRIu64 ",\"total\":%" PRIu64 ",\"reads\":%" PRIu64 ",\"writes\":%" PRIu64 "},"
        "\"memory\":{\"available\":%" PRIu64 ",\"total\":%" PRIu64 "},"
        "\"network\":{\"rx\":%" PRIu64 ",\"tx\":%" PRIu64 "},"
        "\"load\":{\"load1\":%.2f,\"load5\":%.2f,\"load15\":%.2f},"
        "\"hostname\":\"%s\","
        "\"processes\":[",
        time(NULL),

        snapshot.cpu.idle,
        snapshot.cpu.total,
        snapshot.cpu.processes,
        snapshot.process_count,

        snapshot.disk.available,
        snapshot.disk.total,
        snapshot.disk.read,
        snapshot.disk.write,

        snapshot.memory.available,
        snapshot.memory.total,

        snapshot.network.rx,
        snapshot.network.tx,

        snapshot.load.load1,
        snapshot.load.load5,
        snapshot.load.load15,

        info.hostname
    );

    for (unsigned i = 0; i < snapshot.processes_count; i++) {
        if (i != 0) {
            json_len += (size_t)snprintf(json + json_len, sizeof(json) - json_len, ",");
        }

        json_len += (size_t)snprintf(json + json_len, sizeof(json) - json_len,
            "{\"pid\":%d,\"ram\":%" PRIu64 ",\"cpu\":%" PRIu64 ",\"name\":\"%s\"}",
            snapshot.processes[i].pid,
            snapshot.processes[i].ram,
            snapshot.processes[i].cpu,
            snapshot.processes[i].name
        );
    }

    json_len += (size_t)snprintf(json + json_len, sizeof(json) - json_len,
        "],"
        "\"uptime\":%" PRIu64 ","
        "\"temp\":%d"
        "}",
        snapshot.uptime,
        snapshot.temp
    );

    routeJSON(
        socket,
        response,
        response_size,

        json
    );
});

JSON_ROUTE(historyCPU, {
    size_t path_size = BUFFER_ONE_KB;
    char path[path_size];

    size_t time_buffer_size = BUFFER_ONE_KB;
    char time_buffer[time_buffer_size];
    formatTime(cwinfo.started_at, time_buffer, time_buffer_size);

    int _snprintf = snprintf(path, path_size - 1, "%s/history/%s/metric", cwinfo.pulse, time_buffer);
    if(_snprintf < 0) {
        _log(cwinfo, L_ERROR, "snprintf failed for historyCPU");
        return;
    }

    DIR *dir = opendir(path);
    if(!dir) {
        _log(cwinfo, L_ERROR, "Opening DIR failed for path in historyCPU");
        return;
    }

    size_t entry_storer_size = BUFFER_ONE_KB * 8;
    char entry_storer[entry_storer_size];
    entry_storer[0] = '\0';

    struct dirent *entry = readdir(dir);
    while(true) {
        char *name = entry->d_name;
        if(strcmp(name, ".") == 0) {
            entry = readdir(dir);
            continue;
        }
        if(strcmp(name, "..") == 0) {
            entry = readdir(dir);
            continue;
        }

        size_t entry_path_size = BUFFER_ONE_KB;
        char entry_path[entry_path_size];
        int _snprintf = snprintf(entry_path, entry_path_size, "%s/%s", path, name);
        if(_snprintf == -1) {
            _log(cwinfo, L_ERROR, "snprintf failed within while loop for historyCPU");
            break;
        }

        Metrics metric;
        readHistoryM(cwinfo, entry_path, &metric);

        size_t padding = 5;
        size_t len = strlen(entry_storer);

        if (len >= entry_storer_size - padding) {
            size_t drop = len / 2;
            char *boundary = strchr(entry_storer + drop, '}');
            if (boundary != NULL) {
                boundary++;
                if (*boundary == ',') boundary++;

                memmove(entry_storer, boundary, strlen(boundary) + 1);
            } else {
                entry_storer[0] = '\0';
            }

            len = strlen(entry_storer);
        }


        entry = readdir(dir);
        _snprintf = snprintf(
            entry_storer + len,
            entry_storer_size - len,
            "{"
            "\"timestamp\": %" PRIu64 ","
            "\"usage\": %f"
            "}%s",
            strtoul(name, NULL, 10),
            metric.cpuUsage,
            entry != NULL ? "," : ""
        );

        if(_snprintf == -1) {
            _log(cwinfo, L_ERROR, "Finalizing snprintf in while loop failed for historyCPU");
            break;
        }
        if(entry == NULL) break;
        continue;
    }

    char json[BUFFER_ONE_KB * 32];
    _snprintf = snprintf(
        json,
        sizeof(json),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json; charset=UTF-8\r\n\r\n"
        "{"
        "\"error\":null,"
        "\"success\":true,"
        "\"timestamp\":%" PRIu64 ","
        "\"data\": [%s]"
        "}",
        time(NULL),
        entry_storer
    );

    if(_snprintf == -1) {
        _log(cwinfo, L_ERROR, "Finalizing snprintf failed for historyCPU");
        return;
    }

    routeJSON(
        socket,
        response,
        response_size,
        json
    );
});

JSON_ROUTE(historyRAM, {
    size_t path_size = BUFFER_ONE_KB;
    char path[path_size];

    size_t time_buffer_size = BUFFER_ONE_KB;
    char time_buffer[time_buffer_size];
    formatTime(cwinfo.started_at, time_buffer, time_buffer_size);

    int _snprintf = snprintf(path, path_size - 1, "%s/history/%s/metric", cwinfo.pulse, time_buffer);
    if(_snprintf < 0) {
        _log(cwinfo, L_ERROR, "snprintf failed for historyRAM");
        return;
    }

    DIR *dir = opendir(path);
    if(!dir) {
        _log(cwinfo, L_ERROR, "Opening DIR failed form path in historyRAM");
        return;
    }

    size_t entry_storer_size = BUFFER_ONE_KB * 8;
    char entry_storer[entry_storer_size];
    entry_storer[0] = '\0';

    struct dirent *entry = readdir(dir);
    while(true) {
        char *name = entry->d_name;
        if(strcmp(name, ".") == 0) {
            entry = readdir(dir);
            continue;
        }
        if(strcmp(name, "..") == 0) {
            entry = readdir(dir);
            continue;
        }

        size_t entry_path_size = BUFFER_ONE_KB;
        char entry_path[entry_path_size];
        int _snprintf = snprintf(entry_path, entry_path_size, "%s/%s", path, name);
        if(_snprintf < 0) {
            _log(cwinfo, L_ERROR, "snprintf failed within while loop for historyRAM");
            return;
        }

        Metrics metric;
        readHistoryM(cwinfo, entry_path, &metric);
        
        size_t padding = 5;
        size_t len = strlen(entry_storer);

        if (len >= entry_storer_size - padding) {
            size_t drop = len / 2;
            char *boundary = strchr(entry_storer + drop, '}');
            if (boundary != NULL) {
                boundary++;
                if (*boundary == ',') boundary++;

                memmove(entry_storer, boundary, strlen(boundary) + 1);
            } else {
                entry_storer[0] = '\0';
            }

            len = strlen(entry_storer);
        }

        entry = readdir(dir);
        _snprintf = snprintf(
            entry_storer + len,
            entry_storer_size - len,
            "{"
            "\"timestamp\": %" PRIu64 ","
            "\"usage\": %f"
            "}%s",
            strtoul(name, NULL, 10),
            metric.ramUsage,
            entry != NULL ? "," : ""
        );

        if(_snprintf < 0) {
            _log(cwinfo, L_ERROR, "Finalizing snprintf within while loop failed for historyRAM");
            break;
        }
        if(entry == NULL) break;
        continue;
    }

    char json[BUFFER_ONE_KB * 32];
    _snprintf = snprintf(
        json,
        sizeof(json),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json; charset=UTF-8\r\n\r\n"
        "{"
        "\"error\":null,"
        "\"success\":true,"
        "\"timestamp\":%" PRIu64 ","
        "\"data\": [%s]"
        "}",
        time(NULL),
        entry_storer
    );

    if(_snprintf < 0) {
        _log(cwinfo, L_ERROR, "Finalizing snprintf failed for historyRAM");
        return;
    }

    routeJSON(
        socket,
        response,
        response_size,
        json
    );
});

JSON_ROUTE(historyDisk, {
    size_t path_size = BUFFER_ONE_KB;
    char path[path_size];

    size_t time_buffer_size = BUFFER_ONE_KB;
    char time_buffer[time_buffer_size];
    formatTime(cwinfo.started_at, time_buffer, time_buffer_size);

    int _snprintf = snprintf(path, path_size - 1, "%s/history/%s/metric", cwinfo.pulse, time_buffer);
    if(_snprintf < 0) {
        _log(cwinfo, L_ERROR, "snprintf failed for historyDisk");
        return;
    }
    
    DIR *dir = opendir(path);
    if(!dir) {
        _log(cwinfo, L_ERROR, "Opening DIR failed for historyDisk");
        return;
    }

    size_t entry_storer_size = BUFFER_ONE_KB * 8;
    char entry_storer[entry_storer_size];
    entry_storer[0] = '\0';

    struct dirent *entry = readdir(dir);
    while(true) {
        char *name = entry->d_name;
        if(strcmp(name, ".") == 0) {
            entry = readdir(dir);
            continue;
        }
        if(strcmp(name, "..") == 0) {
            entry = readdir(dir);
            continue;
        }

        size_t entry_path_size = BUFFER_ONE_KB;
        char entry_path[entry_path_size];
        int _snprintf = snprintf(entry_path, entry_path_size, "%s/%s", path, name);
        if(_snprintf < 0) {
            _log(cwinfo, L_ERROR, "snprintf within while loop failed for historyDisk");
            break;
        }

        Metrics metric;
        readHistoryM(cwinfo, entry_path, &metric);
        
        size_t padding = 5;
        size_t len = strlen(entry_storer);

        if (len >= entry_storer_size - padding) {
            size_t drop = len / 2;
            char *boundary = strchr(entry_storer + drop, '}');
            if (boundary != NULL) {
                boundary++;
                if (*boundary == ',') boundary++;

                memmove(entry_storer, boundary, strlen(boundary) + 1);
            } else {
                entry_storer[0] = '\0';
            }

            len = strlen(entry_storer);
        }

        entry = readdir(dir);
        _snprintf = snprintf(
            entry_storer + len,
            entry_storer_size - len,
            "{"
            "\"timestamp\": %" PRIu64 ","
            "\"usage\": %f"
            "}%s",
            strtoul(name, NULL, 10),
            metric.diskUsage,
            entry != NULL ? "," : ""
        );

        if(_snprintf < 0) {
            _log(cwinfo, L_ERROR, "Finalizing snprintf within while loop failed for historyDisk");
            break;
        }
        if(entry == NULL) break;
        continue;
    }

    char json[BUFFER_ONE_KB * 32];
    _snprintf = snprintf(
        json,
        sizeof(json),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json; charset=UTF-8\r\n\r\n"
        "{"
        "\"error\":null,"
        "\"success\":true,"
        "\"timestamp\":%" PRIu64 ","
        "\"data\": [%s]"
        "}",
        time(NULL),
        entry_storer
    );

    if(_snprintf < 0) {
        _log(cwinfo, L_ERROR, "Finalizing snprintf failed for historyDisk");
        return;
    }

    routeJSON(
        socket,
        response,
        response_size,

        json
    );
});

void initRoutes(RouteHandler *rh) {
    route("/", indexHtml, GET, rh);
    route("/css/style.css", indexStyle, GET, rh);
    route("/js/script.js", indexJs, GET, rh);
    route("/js/cw.charts.js", indexCwCharts, GET, rh);
    route("/assets/favicon.png", indexFavicon, GET, rh);

    route("/api/metrics", indexMetrics, GET, rh);
    route("/api/history/cpu", historyCPU, GET, rh);
    route("/api/history/ram", historyRAM, GET, rh);
    route("/api/history/disk", historyDisk, GET, rh);
}
