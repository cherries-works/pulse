#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>
#include <sys/select.h>

#include "setup.h"
#include "daemon.h"
#include "render.h"
#include "log.h"

void startRender(Args args, struct CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Starting Renderer"
    );

    struct termios oldt, newt;

    int _tcgetattr = tcgetattr(STDIN_FILENO, &oldt);
    if(_tcgetattr < 0) {
        _log(
            cwinfo,
            L_ERROR,
            "tcgetattr returned less than 0."
        );
    }
    newt = oldt;
    newt.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    int _tcsetattr = tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    if(_tcsetattr < 0) {
        _log(
            cwinfo,
            L_ERROR,
            "tcsetattr returned less than 0."
        );
    }

    System snapshot;
    Metrics metrics;

    bool look_started = false;
    short total_lines = 14 + (short)args.processes;
    
    _log(
        cwinfo,
        L_INFO,
        "(Done) Starting Renderer (while loop started)"
    );

    while(true) {
        if(!look_started) { 
            look_started = true;
        } else {
            clearLines(total_lines);
        }

        readDaemonSM(cwinfo, &snapshot, &metrics);
        render(args, snapshot, metrics);

        fd_set set;
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);

        struct timeval tv = {
            .tv_sec = args.sleep,
            .tv_usec = 0
        };

        int r = select(STDIN_FILENO + 1, &set, NULL, NULL, &tv);
        if (!isatty(STDIN_FILENO)) {
            _log(cwinfo, L_ERROR, "stdin is not a terminal");
            break;
        }

        if (r > 0) {
            char c;
            ssize_t result = read(STDIN_FILENO, &c, 1);
            if(result <= 0) {
                _log(
                    cwinfo,
                    L_ERROR,
                    "Result for read was less than 0."
                );
                break;
            }
            if(c == 'd') {
                _log(
                    cwinfo,
                    L_INFO,
                    "Detaching from Renderer."
                );
                break;
            }
            if(c == 'q') {
                _log(
                    cwinfo,
                    L_INFO,
                    "Qutting Renderer (Process gets stopped)."
                );
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                stop(cwinfo);
                break;
            }
        }
    }

    _log(
        cwinfo,
        L_INFO,
        "(Done) Ending Renderer"
    );

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}