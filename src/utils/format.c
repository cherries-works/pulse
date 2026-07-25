#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

long unsigned unformatTime(char *buffer) {
    const long unsigned year_converter = 60 * 60 * 24 * 30 * 365;
    const long unsigned month_converter = 60 * 60 * 24 * 30;
    const long unsigned day_converter = 60 * 60 * 24;
    long unsigned time = 0;

    char *p = buffer;

    char *minus = strchr(p, '-');
    if(!minus) return 0;
    *minus = '\0';

    long unsigned year = (long unsigned)atoi(p);
    time += (year * year_converter);
    p = minus + 1;

    minus = strchr(p, '-');
    if(!minus) return 0;
    *minus = '\0';

    long unsigned month = (long unsigned)atoi(p);
    time += (month * month_converter);
    p = minus + 1;

    long unsigned day = (long unsigned)atoi(p);
    time += (day * day_converter);

    return time;
}