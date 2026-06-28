#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "parse.h"

unsigned long parseMemoryKey(char *buffer, char *target_key) {
    char *line = buffer;
    char *value = NULL;
    while(*line) {
        // set a pointer to the \n, to the
        // first \name
        //
        char *next = strchr(line, '\n');
        
        // replace \n with a NULL Terminator
        // so (var) line, ends there.
        //
        if(next) *next = '\0';
        else break;

        char *colon = strchr(line, ':');
        if (colon) {
            // do the same technique by replacing
            // the colon. 
            //  eg. MemTotal:   ... kB\n
            //             \0         \0  <- replaced
            //
            *colon = '\0';
            trim(line);

            // compare the line to the key, as it
            // is now enclosed via the NULL pointer
            if (strcmp(line, target_key) == 0) {
                // move the colon pointer one
                // forward so the NULL pointers
                // are wrapped
                //
                value = colon + 1;

                trim(value);
                break;
            }
        } else continue;

        // move the pointer one forward
        // so we skip the NULL pointers
        //
        line = next + 1; 
    }

    if(value == NULL) return 1;
    return strtoull(value, NULL, 10);
}
