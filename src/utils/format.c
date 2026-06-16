#include <stdio.h>
#include <stdlib.h>

void convertTimeInSecondsToString(unsigned seconds, char* buffer) {
    unsigned days = seconds / 3600 / 24;
    unsigned hours = seconds / 3600;
    unsigned minutes = (seconds % 3600) / 60;
    unsigned secs = seconds % 60 % 60 % 60;

    if(days > 0) {
        sprintf(
            buffer, "%dd %dh %dm %ds",
            days,
            hours,
            minutes,
            secs
        );
        return;
    } else if(hours > 0) {
        sprintf(
            buffer, "%dh %dm %ds",
            hours,
            minutes,
            secs
        );
        return;
    } else {
        sprintf(
            buffer, "%dm %ds",
            minutes,
            secs
        );
        return;
    }
}
