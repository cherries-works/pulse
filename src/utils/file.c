#include <stdio.h>

long sizeFile(char *file_name) {
    FILE* file_ptr;
    file_ptr = fopen(file_name, "r");
    if(file_ptr == NULL) return 0;

    fseek(file_ptr, 0L, SEEK_END);
    long size = ftell(file_ptr);
    fclose(file_ptr);
    
    return size;
}

int readFile(char *file_name, size_t size, char *buffer) {
    FILE* file_ptr;
    file_ptr = fopen(file_name, "r");
    if(file_ptr == NULL) return 0;

    size_t file_size = fread(buffer, 1, size - 1, file_ptr);

    fread(buffer, size, 1, file_ptr);
    int c = fclose(file_ptr);
    if(c == 1) return 0;

    buffer[file_size] = '\0';
    return file_size;
}