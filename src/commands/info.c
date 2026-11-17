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
#include "args.h"

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
