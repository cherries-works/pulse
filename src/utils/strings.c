#include <string.h>

#include "utils.h"

void trim(char *buffer) {
    size_t size = strlen(buffer);
    size_t start = 0;
    while(buffer[start] == SPACE_IN_ASCII && start < size) start++;
    
    size_t end = size;
    while(buffer[end] == SPACE_IN_ASCII) end--;


    // move everything to the left side
    size_t i = 0;
    while(i < end - start) {
        buffer[i] = buffer[start + i];
        i++;
    }

    // set everything else to 0
    i = end;
    while(i < size) {
        buffer[i] = 0;
        i++;
    }

    buffer[end - start] = '\0';
}
