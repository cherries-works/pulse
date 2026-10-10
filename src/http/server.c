#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "http.h"
#include "log.h"

Server serverContsructor(
    RouteHandler *routeHandler,
    sa_family_t domain,
    uint16_t port,
    int service,
    int protocol,
    int backlog,
    uint32_t interface,
    void (*launch)(Server *server, struct CWInfo cwinfo)
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
        return server;
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
        return server;
    }

    if(listen(server.socket, server.backlog) < 0) {
        perror("Failed to start listening...\n");
        return server;
    }

    server.launch = launch;
    server.routeHandler = *routeHandler;

    return server;
};

void serverLaunch(Server *server, struct CWInfo cwinfo) {
    size_t buffer_size = BUFFER_ONE_MB;
    char buffer[buffer_size];

    size_t response_size = BUFFER_ONE_KB * 8;
    char response[response_size];

    while(true) {
        struct sockaddr_in client_address;
        socklen_t address_len = sizeof(client_address);
        int new_socket = accept(
            server->socket, 
            (struct sockaddr*)&client_address,
            (socklen_t*)&address_len
        );
        if(new_socket < 0) continue;

        ssize_t bytes_read = read(new_socket, buffer, buffer_size);
        
        if((size_t)bytes_read < buffer_size && bytes_read >= 0) {
            buffer[bytes_read] = '\0';
        } else {
            _log(
                cwinfo,
                L_ERROR,
                "Failed to read buffer"
            );
            continue;;
        }

        Request request = parseRequest(buffer);
        routeHandle(server->routeHandler, request, new_socket, response, response_size, cwinfo);

        close(new_socket);
    }
}
