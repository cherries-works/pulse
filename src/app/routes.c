#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>

#include "parse.h"
#include "http.h"
#include "time.h"
#include "utils.h"
#include "daemon.h"

#define STATIC_ROUTE(fnName, filePath) \
void fnName(int socket, char *response) { \
    routeStatic(socket, response, filePath); \
}

#define JSON_ROUTE(fnName, body) \
void fnName(int socket, char *response) body

STATIC_ROUTE(indexHtml, "./src/app/static/index.html");
STATIC_ROUTE(indexStyle, "./src/app/static/css/style.css");
STATIC_ROUTE(indexJs, "./src/app/static/js/script.js");
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
        fmt,

        (long)time(NULL),

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
    route("/assets/favicon.png", indexFavicon, GET, rh);
    route("/api/metrics", indexMetrics, GET, rh);
}