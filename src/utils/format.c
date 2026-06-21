#include <stdio.h>
#include <stdlib.h>

void convertTimeInSecondsToString(long unsigned seconds, char* buffer) {
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
