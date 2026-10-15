#include <stdio.h>
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

bool startsWith(char *src, size_t src_size, char *comp, size_t comp_size) {
    if(comp_size > src_size) return false;

    for(size_t i = 0; i < comp_size; i++) {
        if(src[i] != comp[i]) return false;
    }

    return true;
}

// escapes a string so it can be put between quotes in a JSON output
void escapeJson(const char *src, char *dst, size_t size) {
    if(size == 0) return;

    size_t j = 0;
    for(size_t i = 0; src[i] != '\0'; i++) {
        unsigned char c = (unsigned char)src[i];
        char escaped[8];

        if(c == '"' || c == '\\') {
            snprintf(escaped, sizeof(escaped), "\\%c", c);
        } else if(c < 0x20) {
            snprintf(escaped, sizeof(escaped), "\\u%04x", c);
        } else {
            escaped[0] = (char)c;
            escaped[1] = '\0';
        }

        // never cut an escape sequence in half
        size_t escaped_len = strlen(escaped);
        if(j + escaped_len >= size) break;

        memcpy(dst + j, escaped, escaped_len);
        j += escaped_len;
    }

    dst[j] = '\0';
}
