#ifndef HTTP_H
#define HTTP_H

#include <netinet/in.h>

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

typedef void (*RouteFn)(
    int socket,
    char *response
);

struct Route {
    char *path;
    enum REQUEST_METHOD method;
    RouteFn handler;
};

struct RouteHandler {
    unsigned short routesAmount;
    struct Route *routes;
};

extern void route(
    char *path, 
    RouteFn handler,
    enum REQUEST_METHOD method, 
    struct RouteHandler *h
);

extern void routeJSON(int new_socket, char *response, char *fmt, ...);
extern void routeStatic(int new_socket, char *response, char *file_path);
extern void handle(struct RouteHandler handler, struct Request request, int new_socket, char *response);

struct Server {
    int domain;
    int port;
    int service;
    int protocol;
    int backlog;
    long interface;

    int socket;
    struct sockaddr_in address;
    struct RouteHandler routeHandler;

    void (*launch)(struct Server *server);
};

extern struct Server serverContsructor(
    struct RouteHandler *routeHandler,
    int domain,
    int port,
    int service,
    int protocol,
    int backlog,
    long interface,
    void (*launch)(struct Server *server)
);

extern void serverLaunch(struct Server *server);
extern void *serverLaunchThread(void *arg);

#endif