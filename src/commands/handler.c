#include "render.h"
#include "log.h"
#include "config.h"
#include "setup.h"
#include "commands.h"
#include "args.h"

void handler(CWArgs cwargs, CWConfig cwconfig, CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Handler is parsing..."
    );

    if (cwargs.no_color) {
        disableColors();
    }

    Command cmd = cwargs.command;

    switch (cmd) {
        case STOP:
            stop(cwinfo);
            break;

        case HELP:
            help(cwinfo);
            break;
        
        case MONITOR:
            monitor(cwargs, cwconfig, cwinfo);
            break;
        
        case INFO:
            info(cwargs, cwinfo);
            break;
        
        case TOP:
            top(cwargs, cwinfo);
            break;
        
        case PROCESS:
            process(cwargs, cwinfo);
            break;
        
        case PRUNE:
            prune(cwargs, cwinfo);
            break;

        case CONFIG:
            config(cwargs, cwinfo);
            break;
        
        case SNAPSHOT:
            snapshot(cwargs, cwinfo);
            break;

        case VERSION:
            version(cwargs);
            break;

        case UPGRADE:
            upgrade(cwargs);
            break;

        default:
            break;
    }

    // cannot log, if we stopped the logger
    if(cmd == STOP) return; 

    _log(
        cwinfo,
        L_INFO,
        "Finished running command"
    );
}
