#include <netinet/in.h>
#include "routes.h"

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