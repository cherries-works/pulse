#include <stdio.h>
#include <string.h>

#include "http.h"
#include "utils.h"

Request parseRequest(char *buffer) {
    char *line = buffer;
    char *split_line = strchr(line, '\n');
    if(split_line) *split_line = '\0'; // \0 byte to finish the first line

    char *space = strchr(line, ' ');
    if(space) *space = '\0'; // to get the method

    REQUEST_METHOD method = GET;
    if (strcmp(line, "POST") == 0) method = POST;
    if (strcmp(line, "DELETE") == 0) method = DELETE;
    if (strcmp(line, "PATCH") == 0) method = PATCH;
    if (strcmp(line, "PUT") == 0) method = PUT;

    line = space + 1;
    space = strchr(line, ' ');
    if(space) *space = '\0'; // to get the path

    Request req = {method, line};
    return req;
}