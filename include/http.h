#ifndef HTTP_H
#define HTTP_H

#include "utils.h"

#include <netinet/in.h>

typedef enum {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
} REQUEST_METHOD;

typedef struct {
    REQUEST_METHOD method;
    char *path;
} Request;

extern Request parseRequest(char *buffer);

typedef void (*RouteFn)(
    int socket,
    char *response,
    size_t response_size
);

typedef struct {
    char *path;
    REQUEST_METHOD method;
    RouteFn handler;
} Route;

typedef struct {
    unsigned short routesAmount;
    Route *routes;
} RouteHandler;

extern void route(
    char *path, 
    RouteFn handler,
    REQUEST_METHOD method, 
    RouteHandler *h
);

extern void routeJSON(int new_socket, char *response, size_t response_size, char *json);
extern void routeStatic(int new_socket, char *file_path, char *response, size_t response_size);
extern void routeHandle(RouteHandler handler, Request request, int new_socket, char *response, size_t response_size);

typedef struct Server {
    int domain;
    int port;
    int service;
    int protocol;
    int backlog;
    long interface;

    int socket;
    struct sockaddr_in address;
    RouteHandler routeHandler;

    void (*launch)(struct Server *server);
} Server;

extern Server serverContsructor(
    RouteHandler *routeHandler,
    sa_family_t domain,
    uint16_t port,
    int service,
    int protocol,
    int backlog,
    uint32_t interface,
    void (*launch)(Server *server)
);

extern void serverLaunch(Server *server);

#endif