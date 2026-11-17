#include <stdio.h>

#include "utils.h"
#include "args.h"

void upgrade(CWArgs cwargs) {
    if(cwargs.json) {
        printf("{\"version\":\"%s\",\"commit\":\"%s\"}\n", PULSE_VERSION, PULSE_COMMIT);
    } else if(cwargs.hash) {
        printf("%s\n", PULSE_COMMIT);
    } else {
        printf("%s\n", PULSE_VERSION);
    }
}