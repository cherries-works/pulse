#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>
#include <sys/select.h>

#include "setup.h"
#include "daemon.h"
#include "render.h"
#include "log.h"

void startRender(Args args) {
    _log(
        L_INFO,
        "Starting Renderer"
    );


    struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    System snapshot;
    Metrics metrics;

    bool look_started = false;
    short total_lines = 13 + (short)args.processes;
    while(running) {
        if(!look_started) { 
            look_started = true;
        } else {
            clearLines(total_lines);
        }

        readDaemonSM(&snapshot, &metrics);
        render(args, snapshot, metrics);

        fd_set set;
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);

        struct timeval tv = {
            .tv_sec = args.sleep,
            .tv_usec = 0
        };

        int r = select(STDIN_FILENO + 1, &set, NULL, NULL, &tv);

        if (r > 0) {
            char c;
            read(STDIN_FILENO, &c, 1);
            if(c == 'd') break;
            if(c == 'q') {
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                failureLog();
                stop();
                return;
            }
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}