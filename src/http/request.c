#include <stdio.h>
#include <string.h>

#include "http.h"

Request parseRequest(char *buffer) {
    Request req = {
        .method = GET,
        .path = "/"
    };

    char *line = buffer;
    char *split_line = strchr(line, '\n');
    if(split_line) *split_line = '\0'; // \0 byte to finish the first line

    char *space = strchr(line, ' ');
    if(space == NULL) return req;
    *space = '\0'; // to get the method

    if (strcmp(line, "POST") == 0) req.method = POST;
    if (strcmp(line, "DELETE") == 0) req.method = DELETE;
    if (strcmp(line, "PATCH") == 0) req.method = PATCH;
    if (strcmp(line, "PUT") == 0) req.method = PUT;

    line = space + 1;
    space = strchr(line, ' ');
    if(space == NULL) return req;
    *space = '\0'; // to get the path

    req.path = line;

    return req;
}