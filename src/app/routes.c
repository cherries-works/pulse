#include <stdio.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "http.h"
#include "daemon.h"
#include "log.h"

#define STATIC_ROUTE(fnName, filePath) \
void fnName(int socket, char *response, size_t response_size) { \
    routeStatic(socket, filePath, response, response_size); \
}

#define JSON_ROUTE(fnName, body) \
void fnName(int socket, char *response, size_t response_size) body

STATIC_ROUTE(indexHtml, "./src/app/static/index.html");
STATIC_ROUTE(indexStyle, "./src/app/static/css/style.css");
STATIC_ROUTE(indexJs, "./src/app/static/js/script.js");
STATIC_ROUTE(indexCwCharts, "./src/app/static/js/cw.charts.js");
STATIC_ROUTE(indexFavicon, "./src/app/static/assets/favicon.png");

JSON_ROUTE(indexMetrics, {
    System snapshot;
    readDaemonS(&snapshot);

    char *fmt =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json; charset=UTF-8\r\n\r\n"
        "{"
        "\"error\":null,"
        "\"success\":true,"
        "\"timestamp\":%ld,"
        "\"cpu\":{\"idle\":%ld,\"total\":%ld,\"processes\":%ld},"
        "\"disk\":{\"available\":%llu,\"total\":%llu, \"reads\":%llu, \"writes\":%llu},"
        "\"memory\":{\"available\":%ld,\"total\":%ld},"
        "\"network\":{\"rx\":%ld,\"tx\":%ld},"
        "\"load\":{\"load1\":%.2f,\"load5\":%.2f,\"load15\":%.2f},"
        "\"processes\": ["
        " {\"pid\":%d,\"ram\":%d,\"cpu\":%d, \"name\": \"%s\"},"
        " {\"pid\":%d,\"ram\":%d,\"cpu\":%d, \"name\": \"%s\"},"
        " {\"pid\":%d,\"ram\":%d,\"cpu\":%d, \"name\": \"%s\"}"
        "],"
        "\"uptime\":%ld"
        "}";

    routeJSON(
        socket,
        response,
        response_size,

        fmt,
        time(NULL),

        snapshot.cpu.idle,
        snapshot.cpu.total,
        snapshot.cpu.processes,

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

        snapshot.processes[0].pid,
        snapshot.processes[0].ram,
        snapshot.processes[0].cpu,
        snapshot.processes[0].name,

        snapshot.processes[1].pid,
        snapshot.processes[1].ram,
        snapshot.processes[1].cpu,
        snapshot.processes[1].name,

        snapshot.processes[2].pid,
        snapshot.processes[2].ram,
        snapshot.processes[2].cpu,
        snapshot.processes[2].name,

        snapshot.uptime
    );
});

JSON_ROUTE(historyCPU, {
    char *home = getenv("HOME");
    if(home == NULL) {
        _log(
            ERROR,
            "No HOME environment variable"
        );
        return;
    }

    size_t time_buffer_size = BUFFER_ONE_KB / 8;
    char time_buffer[time_buffer_size];

    time_t _time = getCurrentLog();
    formatTime(_time, time_buffer, time_buffer_size);

    size_t path_size = BUFFER_ONE_KB;
    char path[path_size];

    snprintf(
        path, path_size, 
        "%s/%s/history/%s/metric", 
        home,
        R_CHERRIES_FOLDER_PULSE, 
        time_buffer
    );

    
    DIR *dir = opendir(path);
    if(!dir) {
        _log(ERROR, "History file for system not found.");
        return;
    }

    size_t entry_storer_size = BUFFER_ONE_KB * 4;
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
        snprintf(entry_path, entry_path_size, "%s/%s", path, name);

        Metrics metric;
        readHistoryM(entry_path, &metric);
        
        size_t len = strlen(entry_storer);
        if(len >= entry_storer_size) break;

        entry = readdir(dir);
        int written = snprintf(
            entry_storer + len,
            entry_storer_size - len,
            "{"
            "\"timestamp\": %ld,"
            "\"usage\": %f"
            "}%s",
            strtoul(name, NULL, 10),
            metric.cpuUsage,
            entry != NULL ? "," : ""
        );

        if(written < 0) break;
        if(entry == NULL) break;
        continue;
    }

    char *fmt =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json; charset=UTF-8\r\n\r\n"
        "{"
        "\"error\":null,"
        "\"success\":true,"
        "\"timestamp\":%ld,"
        "\"data\": [%s]"
        "}";

    routeJSON(
        socket,
        response,
        response_size,

        fmt,

        time(NULL),
        entry_storer
    );
});

JSON_ROUTE(historyRAM, {
    char *home = getenv("HOME");
    if(home == NULL) {
        _log(
            ERROR,
            "No HOME environment variable"
        );
        return;
    }

    size_t time_buffer_size = BUFFER_ONE_KB / 8;
    char time_buffer[time_buffer_size];

    time_t _time = getCurrentLog();
    formatTime(_time, time_buffer, time_buffer_size);

    size_t path_size = BUFFER_ONE_KB;
    char path[path_size];

    snprintf(
        path, path_size, 
        "%s/%s/history/%s/metric", 
        home,
        R_CHERRIES_FOLDER_PULSE, 
        time_buffer
    );

    
    DIR *dir = opendir(path);
    if(!dir) {
        _log(ERROR, "History file for system not found.");
        return;
    }

    size_t entry_storer_size = BUFFER_ONE_KB * 4;
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
        snprintf(entry_path, entry_path_size, "%s/%s", path, name);

        Metrics metric;
        readHistoryM(entry_path, &metric);
        
        size_t len = strlen(entry_storer);
        if(len >= entry_storer_size) break;

        entry = readdir(dir);
        int written = snprintf(
            entry_storer + len,
            entry_storer_size - len,
            "{"
            "\"timestamp\": %ld,"
            "\"usage\": %f"
            "}%s",
            strtoul(name, NULL, 10),
            metric.ramUsage,
            entry != NULL ? "," : ""
        );

        if(written < 0) break;
        if(entry == NULL) break;
        continue;
    }

    char *fmt =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json; charset=UTF-8\r\n\r\n"
        "{"
        "\"error\":null,"
        "\"success\":true,"
        "\"timestamp\":%ld,"
        "\"data\": [%s]"
        "}";

    routeJSON(
        socket,
        response,
        response_size,

        fmt,

        time(NULL),
        entry_storer
    );
});

JSON_ROUTE(historyDisk, {
    char *home = getenv("HOME");
    if(home == NULL) {
        _log(
            ERROR,
            "No HOME environment variable"
        );
        return;
    }

    size_t time_buffer_size = BUFFER_ONE_KB / 8;
    char time_buffer[time_buffer_size];

    time_t _time = getCurrentLog();
    formatTime(_time, time_buffer, time_buffer_size);

    size_t path_size = BUFFER_ONE_KB;
    char path[path_size];

    snprintf(
        path, path_size, 
        "%s/%s/history/%s/metric", 
        home,
        R_CHERRIES_FOLDER_PULSE, 
        time_buffer
    );

    
    DIR *dir = opendir(path);
    if(!dir) {
        _log(ERROR, "History file for system not found.");
        return;
    }

    size_t entry_storer_size = BUFFER_ONE_KB * 4;
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
        snprintf(entry_path, entry_path_size, "%s/%s", path, name);

        Metrics metric;
        readHistoryM(entry_path, &metric);
        
        size_t len = strlen(entry_storer);
        if(len >= entry_storer_size) break;

        entry = readdir(dir);
        int written = snprintf(
            entry_storer + len,
            entry_storer_size - len,
            "{"
            "\"timestamp\": %ld,"
            "\"usage\": %f"
            "}%s",
            strtoul(name, NULL, 10),
            metric.diskUsage,
            entry != NULL ? "," : ""
        );

        if(written < 0) break;
        if(entry == NULL) break;
        continue;
    }

    char *fmt =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json; charset=UTF-8\r\n\r\n"
        "{"
        "\"error\":null,"
        "\"success\":true,"
        "\"timestamp\":%ld,"
        "\"data\": [%s]"
        "}";

    routeJSON(
        socket,
        response,
        response_size,

        fmt,

        time(NULL),
        entry_storer
    );
});

JSON_ROUTE(indexRAM, {
    System snapshot;
    readDaemonS(&snapshot);

    char *fmt =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json; charset=UTF-8\r\n\r\n"
        "{"
        "\"error\":null,"
        "\"success\":true,"
        "\"timestamp\":%ld,"
        "\"cpu\":{\"idle\":%ld,\"total\":%ld,\"processes\":%ld},"
        "\"disk\":{\"available\":%llu,\"total\":%llu, \"reads\":%llu, \"writes\":%llu},"
        "\"memory\":{\"available\":%ld,\"total\":%ld},"
        "\"network\":{\"rx\":%ld,\"tx\":%ld},"
        "\"load\":{\"load1\":%.2f,\"load5\":%.2f,\"load15\":%.2f},"
        "\"processes\": ["
        " {\"pid\":%d,\"ram\":%d,\"cpu\":%d, \"name\": \"%s\"},"
        " {\"pid\":%d,\"ram\":%d,\"cpu\":%d, \"name\": \"%s\"},"
        " {\"pid\":%d,\"ram\":%d,\"cpu\":%d, \"name\": \"%s\"}"
        "],"
        "\"uptime\":%ld"
        "}";

    routeJSON(
        socket,
        response,
        response_size,

        fmt,
        time(NULL),

        snapshot.cpu.idle,
        snapshot.cpu.total,
        snapshot.cpu.processes,

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

        snapshot.processes[0].pid,
        snapshot.processes[0].ram,
        snapshot.processes[0].cpu,
        snapshot.processes[0].name,

        snapshot.processes[1].pid,
        snapshot.processes[1].ram,
        snapshot.processes[1].cpu,
        snapshot.processes[1].name,

        snapshot.processes[2].pid,
        snapshot.processes[2].ram,
        snapshot.processes[2].cpu,
        snapshot.processes[2].name,

        snapshot.uptime
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