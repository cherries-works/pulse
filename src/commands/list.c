#include <stdio.h>

#include "http.h"

void list(CWArgs cwargs) {
    if(cwargs.json) {
        printf("{\"version\":\"%s\",\"commit\":\"%s\"}\n", PULSE_VERSION, PULSE_COMMIT);
    } else if(cwargs.hash) {
        printf("%s\n", PULSE_COMMIT);
    } else {
        printf("%s\n", PULSE_VERSION);
    }
}