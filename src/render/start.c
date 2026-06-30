#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "setup.h"
#include "daemon.h"
#include "render.h"
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
        sleep(args.sleep);
    }
}