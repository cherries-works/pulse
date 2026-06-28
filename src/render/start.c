#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <stdbool.h>

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"
#include "setup.h"

#include "app.h"
#include "setup.h"
#include "utils.h"
#include "daemon.h"
#include "log.h"


void startRender(PulseArgs args) {
    _log(
        INFO,
        "Starting Renderer"
    );


    System snapshot;
    Metrics metrics;

    bool look_started = false;
    short total_lines = 15;
    while(running) {
        if(!look_started) { 
            look_started = true;
        } else {
            clearLines(total_lines);
        }

        readDaemonSM(&snapshot, &metrics);
        render(args, snapshot, metrics);
        sleep(1);
    }
}