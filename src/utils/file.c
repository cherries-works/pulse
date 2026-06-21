#include <stdio.h>

size_t sizeFile(const char *file_name) {
    FILE* file_ptr;
    file_ptr = fopen(file_name, "r");
    if(file_ptr == NULL) return 0;

    fseek(file_ptr, 0L, SEEK_END);
    size_t size = (size_t)ftell(file_ptr);
    
    fclose(file_ptr);
    return size;
}

size_t readFile(const char *file_name, size_t size, char *buffer) {
    if(size == 0) return 0;

    FILE* file_ptr;
    file_ptr = fopen(file_name, "r");
    if(file_ptr == NULL) {
        buffer[0] = '\0';
        return 0;
    }

    size_t file_size = fread(buffer, 1, size - 1, file_ptr);

    fclose(file_ptr);
    buffer[file_size] = '\0';
    return file_size;
}