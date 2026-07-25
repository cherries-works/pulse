#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include "utils.h"

void cleanDir(char *path) {
    if(strlen(path) == 0) return;
    if(strcmp(path, ".") == 0) return;
    if(strcmp(path, "..") == 0) return;
    if(strcmp(path, "/") == 0) return;

    struct dirent *entry;
    DIR *dir = opendir(path);
    if(!dir) return;

    char newPath[BUFFER_ONE_KB];
    while((entry = readdir(dir)) != NULL) {
        size_t len = strlen(path) + strlen(entry->d_name) + 1; // dir length + file name length + /
        if(strcmp(entry->d_name, ".") == 0) continue;
        if(strcmp(entry->d_name, "..") == 0) continue;

        sprintf(newPath, "%s/%s", path, entry->d_name);
        newPath[len] = '\0';

        if(entry->d_type == DT_DIR) {
            cleanDir(newPath);
            continue;
        } else if(entry->d_type == DT_REG) {
            remove(newPath);
            continue;
        }
    }

    closedir(dir);
    rmdir(path);

    return;
}

unsigned countDir(char *path) {
    unsigned file_count = 0;

    if(strlen(path) == 0) return file_count;
    if(strcmp(path, ".") == 0) return file_count;
    if(strcmp(path, "..") == 0) return file_count;
    if(strcmp(path, "/") == 0) return file_count;


    struct dirent *entry;
    DIR *dir = opendir(path);
    if(!dir) return file_count;

    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0) continue;
        if(strcmp(entry->d_name, "..") == 0) continue;
        file_count++;
        continue;
    }

    return file_count;
}