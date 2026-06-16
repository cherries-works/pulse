#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <time.h>

#include "utils.h"
#include "parse.h"
#include "http.h"

struct Load getLoad(size_t size, char *buffer) {
    char *line = buffer;
    char *next = strchr(line, SPACE_IN_ASCII);
    *next = '\0';

    float load1 = atof(line);

    line = next + 1;
    next = strchr(line, SPACE_IN_ASCII);
    *next = '\0';

    float load5 = atof(line);

    line = next + 1;
    next = strchr(line, SPACE_IN_ASCII);
    *next = '\0';

    float load15 = atof(line);

    struct Load snapshot = { load1, load5, load15 };
    return snapshot;
}

