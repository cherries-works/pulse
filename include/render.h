#ifndef RENDER_H
#define RENDER_H

#include <stdlib.h>
#include "utils.h"

extern void startRender(Args args);
extern void render(
    Args args,
    System system,
    Metrics metrics
);

extern void printProcess(
    Process process,
    System system
);

extern void printProcessExtra(
    Process process,
    System system
);

extern char *getBars(float val);
extern char* getColor(float value);

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