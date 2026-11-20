#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>

#include "setup.h"


typedef enum {
    CPU,
    RAM
} Sort;

typedef enum {
    LOGS,
    HISTORY,
    ALL
} Prune;

typedef enum {
    MONITOR,
    TOP,
    INFO,
    STOP,
    HELP,
    PROCESS,
    PRUNE,
    CONFIG,
    SNAPSHOT,
    VERSION,
    WATCH,
    REMOTE,
    UPGRADE,
} Command;

typedef struct {
    bool resources;
    bool load;
    bool network;
    bool io;
    bool processes;
} Display;

typedef struct {
    Command command;
   
    Display display; // for the MONITOR command

    bool web; // for the MONITOR command
    bool headless; // for the MONITOR command
    
    bool json; // for the INFO / SNAPSHOT / TOP / PROCESS / PRUNE / CONFIG / VERSION command
    
    bool reset; // for the CONFIG command
    
    bool hash; // for the VERSION command
    
    bool current; // for the CONFIG command

    bool no_color; // for MONITOR / INFO / HELP / SNAPSHOT / TOP / PROCESS / WATCH command 

    Sort sort; // for the MONITOR / TOP command

    uint8_t style; // for the MONITOR command (1/2/3)

    uint8_t processes; // for the MONITOR / TOP command
    uint16_t port; // for the MONITOR command
    float sleep; // for the MONITOR command
    float history_snapshot_interval; // for the MONITOR command
 
    pid_t pid; // for the PROCESS / WATCH command
    char name[256]; // for the PROCESS / WATCH command

    Prune prune; // for the PRUNE command
    uint8_t keep; // for the PRUNE command
    char until[64]; // for the PRUNE command
} CWArgs;

extern CWArgs parseArgs(CWInfo cwinfo, int argc, char* argv[]);

#endif