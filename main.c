#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>
#include <wait.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <termios.h>

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"
#include "daemon.h"
#include "log.h"

#include "app.h"
#include "setup.h"
#include "config.h"

struct CWInfo *_cwinfo = NULL;

pid_t render_pid = 0;
struct termios oldt;
void term(int sig) {
    if(getpid() == render_pid) {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }

    if(_cwinfo != NULL) {
        stop(*_cwinfo);
    }
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    render_pid = getpid();

    tcgetattr(STDIN_FILENO, &oldt);
    signal(SIGINT, term);

    struct CWInfo cwinfo = setup();
    _cwinfo = &cwinfo;

    Args args = parseArgs(cwinfo, argc, argv);
    Config config = parseToml(cwinfo);
    handle(args, config, cwinfo);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}

