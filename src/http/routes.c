#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <time.h>

#include "http.h"

char *getFileExtension(char *file_path) {
    char *dot = strrchr(file_path, '.');
    if(!dot || dot == file_path) return "";
    return dot + 1;
}

void routeNotFound(
    int new_socket, 
    char *response,
    size_t response_size
) {
    snprintf(
        response,
        response_size,
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: application/json; charset=UTF-8\r\n\r\n"
        "{"
        "\"error\": \"Not Found\","
        "\"success\":false,"
        "\"timestamp\":%ld"
        "}",
        time(NULL)
    );

    write(new_socket, response, strlen(response));
}

void routeInvalidMethod(int new_socket, char *response, size_t response_size) {
    snprintf(
        response,
        response_size,
        "HTTP/1.1 400 Bad Request\r\n"
        "Content-Type: application/json; charset=UTF-8\r\n\r\n"
        "{"
        "\"error\": \"Invalid Method\","
        "\"success\":false,"
        "\"timestamp\":%ld"
        "}",
        time(NULL)
    );

    write(new_socket, response, strlen(response));
}

void routeHTML(char *file_path, int new_socket, char *response, size_t response_size) {
    size_t file_buffer_size = BUFFER_ONE_KB * 16;
    char file_buffer[file_buffer_size];
    
    size_t file_size = sizeFile(file_path);
    snprintf(
        response,
        response_size,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %ld\r\n\r\n",
        file_size
    );
    write(new_socket, response, strlen(response));
    
    FILE *file_ptr = fopen(file_path, "r");
    size_t size = fread(file_buffer, 1, file_buffer_size - 1, file_ptr);
    file_buffer[size] = '\0';
    
    fclose(file_ptr);
    write(new_socket, file_buffer, size);
}

void routeCSS(char *file_path, int new_socket, char *response, size_t response_size) {
    size_t file_buffer_size = BUFFER_ONE_KB * 16;
    char file_buffer[file_buffer_size];

    size_t file_size = sizeFile(file_path);
    snprintf(
        response,
        response_size,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/css\r\n"
        "Content-Length: %ld\r\n\r\n",
        file_size
    );
    write(new_socket, response, strlen(response));

    FILE *file_ptr = fopen(file_path, "r");
    size_t size = fread(file_buffer, 1, file_buffer_size - 1, file_ptr);
    file_buffer[size] = '\0';

    fclose(file_ptr);
    write(new_socket, file_buffer, size);
}

void routeJS(char *file_path, int new_socket, char *response, size_t response_size) {
    size_t file_buffer_size = BUFFER_ONE_KB * 16;
    char file_buffer[file_buffer_size];

    size_t file_size = sizeFile(file_path);
    snprintf(
        response,
        response_size,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/js\r\n"
        "Content-Length: %ld\r\n\r\n",
        file_size
    );
    write(new_socket, response, strlen(response));

    FILE *file_ptr = fopen(file_path, "r");
    size_t size = fread(file_buffer, 1, file_buffer_size - 1, file_ptr);
    file_buffer[size] = '\0';

    fclose(file_ptr);
    write(new_socket, file_buffer, size);
}

void routeImage(char *file_path, int new_socket, char *response, size_t response_size) {
    size_t file_buffer_size = BUFFER_ONE_KB * 128;
    char file_buffer[file_buffer_size];

    size_t file_size = sizeFile(file_path);
    snprintf(
        response,
        response_size,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: image/png\r\n"
        "Content-Length: %ld\r\n\r\n",
        file_size
    );
    write(new_socket, response, strlen(response));

    FILE *file_ptr = fopen(file_path, "r");
    size_t size = fread(file_buffer, 1, file_buffer_size - 1, file_ptr);
    file_buffer[size] = '\0';

    fclose(file_ptr);
    write(new_socket, file_buffer, file_size);
}

void routeJSON(
    int new_socket, 
    char *response,
    size_t response_size,
    char *json
) {
    snprintf(
        response,
        response_size,
        json,
        NULL
    );

    write(new_socket, response, strlen(response));
}

void routeStatic(
    int new_socket,
    char *file_path,
    char *response,
    size_t response_size
) {
    char *ext = getFileExtension(file_path);

    if(strcmp(ext, "html") == 0) routeHTML(file_path, new_socket, response, response_size);
    else if(strcmp(ext, "css") == 0) routeCSS(file_path, new_socket, response, response_size);
    else if(strcmp(ext, "js") == 0) routeJS(file_path, new_socket, response, response_size);
    else if(strcmp(ext, "png") == 0) routeImage(file_path, new_socket, response, response_size);
    else routeNotFound(new_socket, response, response_size);
}

void route(
    char *path,
    RouteFn handler,
    REQUEST_METHOD method,
    RouteHandler *h
) {
    Route r = { 
        .path = path,
        .handler = handler,
        .method = method,
    };
    
    h->routes[h->routesAmount] = r;
    h->routesAmount += 1;
}

void handle(
    RouteHandler handler,
    Request request,
    int new_socket,
    char *response,
    size_t response_size
) {
    for(int i = 0; i < handler.routesAmount; i++) {
        Route r = handler.routes[i];

        if(request.method != r.method) continue;
        if(strcmp(request.path, r.path) != 0) continue;

        r.handler(new_socket, response, response_size);
        return;
    }

    routeNotFound(new_socket, response, response_size);
    return;
}