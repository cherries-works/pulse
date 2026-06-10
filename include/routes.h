#include "request.h"

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
