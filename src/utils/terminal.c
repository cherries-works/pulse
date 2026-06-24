#include <stdio.h>

void clearLine() {
    printf("\033[F\033[K");
}

void clearLines(short i) {
    while(i > 0) {
        printf("\033[F\033[K");
        i--;
    }
}