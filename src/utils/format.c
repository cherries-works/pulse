#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

void formatTimeHumanReadable(
    long unsigned seconds, 
    char* buffer,
    size_t size
) {
    long unsigned days = seconds / 3600 / 24;
    long unsigned hours = seconds / 3600;
    long unsigned minutes = (seconds % 3600) / 60;
    long unsigned secs = seconds % 60 % 60 % 60;

    if(days > 0) {
        snprintf(
            buffer, size, 
            "%ldd %ldh %ldm %lds",
            days,
            hours,
            minutes,
            secs
        );
        return;
    } else if(hours > 0) {
        snprintf(
            buffer, size, 
            "%ldh %ldm %lds",
            hours,
            minutes,
            secs
        );
        return;
    } else {
        snprintf(
            buffer, size, 
            "%ldm %lds",
            minutes,
            secs
        );
        return;
    }
}

void formatTime(time_t _time, char *buffer, size_t size) {
    struct tm tm_info;
    localtime_r(&_time, &tm_info);

    strftime(buffer, size, "%Y-%m-%d--%H:%M:%S", &tm_info);
}