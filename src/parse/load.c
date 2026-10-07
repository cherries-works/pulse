#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "parse.h"
#include "log.h"

Load getLoad(size_t size, char *buffer) {
    _log(L_INFO, "Getting load info.");

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

    _log(L_INFO, "(Done) Getting load info.");
    return load;
}

