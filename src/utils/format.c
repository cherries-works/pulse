#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

void formatTimeHumanReadable(long unsigned seconds, char* buffer) {
    long unsigned days = seconds / 3600 / 24;
    long unsigned hours = seconds / 3600;
    long unsigned minutes = (seconds % 3600) / 60;
    long unsigned secs = seconds % 60 % 60 % 60;

    if(days > 0) {
        sprintf(
            buffer, "%ldd %ldh %ldm %lds",
            days,
            hours,
            minutes,
            secs
        );
        return;
    } else if(hours > 0) {
        sprintf(
            buffer, "%ldh %ldm %lds",
            hours,
            minutes,
            secs
        );
        return;
    } else {
        sprintf(
            buffer, "%ldm %lds",
            minutes,
            secs
        );
        return;
    }
}

void formatTime(char *buffer, size_t size, time_t _time) {
    struct tm tm_info;
    localtime_r(&_time, &tm_info);

    strftime(buffer, size, "%Y-%m-%d--%H:%M:%S", &tm_info);
}