#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "parse.h"

Load getLoad(char *buffer) {
    Load load = { 0.0f, 0.0f, 0.0f };
    char *line = buffer;

    char *next = strchr(line, SPACE_IN_ASCII);
    if(next) *next = '\0';
    else return load;

    load.load1 = (float)atof(line);

    line = next + 1;
    next = strchr(line, SPACE_IN_ASCII);
    if(next) *next = '\0';
    else return load;

    load.load5 = (float)atof(line);

    line = next + 1;
    next = strchr(line, SPACE_IN_ASCII);
    if(next) *next = '\0';
    else return load;

    load.load15 = (float)atof(line);
    return load;
}

