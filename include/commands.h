#ifndef COMMANDS_H
#define COMMANDS_H

#include "config.h"
#include "args.h"

extern void handler(CWArgs cwargs, CWConfig cwconfig, CWInfo cwinfo);
extern void stop(CWInfo cwinfo);
extern void help(CWInfo cwinfo);
extern void top(CWArgs cwargs, CWInfo cwinfo);
extern void info(CWArgs cwargs, CWInfo cwinfo);
extern void monitor(CWArgs cwargs, CWConfig cwconfig, CWInfo cwinfo);
extern void process(CWArgs cwargs, CWInfo cwinfo);
extern void prune(CWArgs cwargs, CWInfo cwinfo);
extern void config(CWArgs cwargs, CWInfo cwinfo);
extern void snapshot(CWArgs cwargs, CWInfo cwinfo);
extern void version(CWArgs cwargs);
extern void handle(CWArgs cwargs, CWConfig cwconfig, CWInfo cwinfo);

#endif