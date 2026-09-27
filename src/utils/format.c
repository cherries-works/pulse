#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>

void formatTimeHumanReadable(
    uint64_t seconds, 
    char* buffer,
    size_t size
) {
    uint64_t days = seconds / 3600 / 24;
    uint64_t hours = (seconds / 3600) % 24;
    uint64_t minutes = (seconds % 3600) / 60;
    uint64_t secs = seconds % 60 % 60 % 60;

    if(days > 0) {
        snprintf(
            buffer, size, 
            "%" PRIu64 "d %" PRIu64 "h %" PRIu64 "m %" PRIu64 "s",
            days,
            hours,
            minutes,
            secs
        );
        return;
    } else if(hours > 0) {
        snprintf(
            buffer, size, 
            "%" PRIu64 "h %" PRIu64 "m %" PRIu64 "s",
            hours,
            minutes,
            secs
        );
        return;
    } else {
        snprintf(
            buffer, size, 
            "%" PRIu64 "m %" PRIu64 "s",
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

uint64_t unformatTime(char *buffer) {
    const uint64_t year_converter = 60 * 60 * 24 * 30 * 365;
    const uint64_t month_converter = 60 * 60 * 24 * 30;
    const uint64_t day_converter = 60 * 60 * 24;
    uint64_t time = 0;

    char *p = buffer;

    char *minus = strchr(p, '-');
    if(!minus) return 0;
    *minus = '\0';

    uint64_t year = (uint64_t)atoi(p);
    time += (year * year_converter);
    p = minus + 1;

    minus = strchr(p, '-');
    if(!minus) return 0;
    *minus = '\0';

    uint64_t month = (uint64_t)atoi(p);
    time += (month * month_converter);
    p = minus + 1;

    uint64_t day = (uint64_t)atoi(p);
    time += (day * day_converter);

    return time;
}
