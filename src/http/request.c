#include <stdio.h>
#include <string.h>

#include "http.h"

Request parseRequest(char *buffer) {
    Request req = {
        .method = GET,
        .path = "/"
    };

    if(buffer == NULL) return req;

    char *line = buffer;
    char *split_line = strchr(line, '\n');
    if(split_line) *split_line = '\0';

    char *space = strchr(line, ' ');
    if(space == NULL) return req;
    
    int method_len = (int)(space - line);
    if(method_len > 16 || method_len <= 0) return req;
    
    *space = '\0';

    if (strcmp(line, "GET") == 0) req.method = GET;
    else if (strcmp(line, "POST") == 0) req.method = POST;
    else if (strcmp(line, "DELETE") == 0) req.method = DELETE;
    else if (strcmp(line, "PATCH") == 0) req.method = PATCH;
    else if (strcmp(line, "PUT") == 0) req.method = PUT;
    else return req;

    line = space + 1;
    space = strchr(line, ' ');
    if(space == NULL) return req;
    
    int path_len = (int)(space - line);
    if(path_len > 2048 || path_len <= 0) return req;
    
    *space = '\0';

    req.path = line;

    return req;
}