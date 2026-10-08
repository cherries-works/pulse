#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "parse.h"
#include "log.h"

// reading the first number
uint64_t parseUptime(size_t size, char *buffer) {
    unsigned i = 0;

    // should be maximum 16 characters
    char number[16];
    while(i < size) {
        char c = buffer[i];
        if(c == '\0') {
            break;
        }
        if(c == ' ') {
            break;
        }

        number[i] = c;
        i++;
    }

    uint64_t n = strtoull(number, NULL, 10);
    return n;
}
