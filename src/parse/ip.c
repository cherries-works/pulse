#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "parse.h"

void _parseIpv6(char *hostname, IP *ip) {
    struct addrinfo hints_ipv6 = {
        .ai_family = AF_INET6,
        .ai_socktype = SOCK_STREAM
    };

    struct addrinfo *result;
    int status = getaddrinfo(hostname, NULL, &hints_ipv6, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return;
    }

    for (struct addrinfo *p = result; p != NULL; p = p->ai_next) {
        if(p->ai_family == AF_INET6) {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            inet_ntop(AF_INET6, &ipv6->sin6_addr, ip->ipv6, sizeof(ip->ipv6));
        } else continue;
    }

    freeaddrinfo(result);
}

void _parseIpv4(char *hostname, IP *ip) {
    struct addrinfo hints_ipv4 = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM
    };

    struct addrinfo *result;
    int status = getaddrinfo(hostname, NULL, &hints_ipv4, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return;
    }

    for (struct addrinfo *p = result; p != NULL; p = p->ai_next) {
        if(p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            inet_ntop(AF_INET, &ipv4->sin_addr, ip->ipv4, sizeof(ip->ipv4));
        } else continue;
    }

    freeaddrinfo(result);
}

IP parseIp(char *hostname) {
    IP ip = {
        .ipv4 = "",
        .ipv6 = ""
    };

    _parseIpv4(hostname, &ip);
    _parseIpv6(hostname, &ip);

    return ip;
}