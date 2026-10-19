#ifndef RENDER_H
#define RENDER_H

#include <stdlib.h>
#include "utils.h"

extern void startRender(Args args, CWInfo cwinfo);
extern void render(
    Args args,
    System system,
    Metrics metrics
);
extern void renderSnapshot(
    Args args,
    System system,
    Metrics metrics
);
extern void renderInfo(
    Args args,
    System system,
    Info info
);

extern void printProcess(
    Process process,
    System system
);

extern void printProcessExtra(
    Process process,
    System system
);

extern char *statuses[];

extern char *getBars(float val);
extern char* getColor(float value);
extern void disableColors(void);

extern char *RED;
extern char *GREEN;
extern char *YELLOW;
extern char *BLUE;
extern char *WHITE;

extern char *BOLD;
extern char *DIM;
extern char *ITALIC;
extern char *UNDERLINE;

extern char *RESET;

#endif