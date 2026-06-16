#include <stdio.h>
#include <string.h>

#include "utils.h"

void trim(char *buffer) {
    unsigned size = strlen(buffer);
    unsigned start = 0;
    while(buffer[start] == SPACE_IN_ASCII && start < size) start++;
    
    unsigned end = size;
    while(buffer[end] == SPACE_IN_ASCII && end >= 0) end--;


    // move everything to the left side
    unsigned i = 0;
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
