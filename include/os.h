#ifndef OS_H
#define OS_H

typedef enum {
    OS_LINUX,
    OS_MACOS,
    OS_WINDOWS_BSD,
    OS_UNKNOWN
} OsType;

static inline OsType detect_os() {
    #if defined(__linux__)
        return OS_LINUX;
    #elif defined(__APPLE__)
        return OS_MACOS;
    #elif defined(_WIN32)
        return OS_WINDOWS_BSD;
    #else
        return OS_UNKNOWN;
    #endif
}

#endif