#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <time.h>

#include "utils.h"
#include "parse.h"
#include "http.h"

Server serverContsructor(
    RouteHandler *routeHandler,
    int domain,
    int port,
    int service,
    int protocol,
    int backlog,
    long interface,
    void (*launch)(Server *server)
) {
    Server server;

    server.domain = domain;
    server.port = port;
    server.service = service;
    server.protocol = protocol;
    server.backlog = backlog;

    server.address.sin_family = domain;
    server.address.sin_port = htons(port);
    server.address.sin_addr.s_addr = htonl(interface);

    server.socket = socket(domain, service, protocol);
    if(server.socket < 0) {
        perror("Failed to initialize/connect to socket...\n");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(
        server.socket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );

    if (bind(server.socket, (struct sockaddr*)&server.address, sizeof(server.address)) < 0) {
        perror("Failed to bind socket...\n");
        exit(EXIT_FAILURE);
    }

    if(listen(server.socket, server.backlog) < 0) {
        perror("Failed to start listening...\n");
        exit(EXIT_FAILURE);
    }

    server.launch = launch;
    server.routeHandler = *routeHandler;

    return server;
};

void serverLaunch(Server *server) {
    unsigned long bufferSize = BUFFER_ONE_MB;
    char buffer[bufferSize];

    unsigned long responseSize = BUFFER_ONE_KB * 8;
    char response[responseSize];

    while(1) {
        int addrLen = sizeof(server->address);
        int newSocket = accept(server->socket, (struct sockaddr*)&server->address, (socklen_t*)&addrLen);
        ssize_t bytesRead = read(newSocket, buffer, bufferSize);
        
        if(bytesRead < bufferSize && bytesRead >= 0) {
            buffer[bytesRead] = '\0';
        } else {
            printf("Error reading buffer...\n");
        }

        Request request = parseRequest(buffer);
        handle(server->routeHandler, request, newSocket, response);

        close(newSocket);
    }
}

void *serverLaunchThread(void *arg) {
    serverLaunch(arg);
    return NULL;
}