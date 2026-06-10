#ifndef REQUEST_H
#define REQUEST_H

enum REQUEST_METHOD {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
};

struct Request {
    enum REQUEST_METHOD method;
    char *path;
};

extern struct Request parseRequest(char *buffer);

#endif