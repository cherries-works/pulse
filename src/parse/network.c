#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <time.h>

#include "utils.h"
#include "parse.h"
#include "http.h"

Network getNetwork(char *buffer) {
    unsigned long rx = 0;
    unsigned long tx = 0;

    char *line = buffer;
    char *next = strchr(line, '\n');
    if(next) {
        line = next + 1;
        next = strchr(line, '\n');
        if(next) {
            line = next + 1;
        } else {
            line = line + strlen(line);
        }
    } else {
        line = line + strlen(line);
    }
    
    // Inter-|   Receive           |  Transmit                          \n <--- first strchr
    // face  |bytes  ..  multicast|bytes    packets .... compressed     \n <--- second strchr
    //                                                                  So we start at the start of lo
    // lo:   38914   ..  38914     307    0    ....                0
    // enp2s0:    0 ....         0        0       ....               0
    // wlp0s20f3: 183282384 ....         0 18357363   ....       0          0

    next = strchr(line, ':');
    if(next) line = next + 1;
    else line = line + strlen(line);

    while(*line) {
        // 8 total keys per face (for rx AND tx)
        int KEYS_AMOUNT = 8;
        for(int i = 0; i < KEYS_AMOUNT; i++) {
            while (*line == ' ') line++;

            next = strchr(line, ' ');
            if(next) *next = '\0';
            else continue;

            rx += strtoull(line, NULL, 10);
            if(next) line = next + 1;
            else line = line + strlen(line);
        }

        for(int i = 0; i < KEYS_AMOUNT; i++) {
            while (*line == ' ') line++;

            next = strchr(line, ' ');
            if(next) *next = '\0';
            else continue;

            tx += strtoull(line, NULL, 10);
            
            if(next) line = next + 1;
            else line = line + strlen(line);
        }

        next = strchr(line, '\n');
        if(next) line = next + 1;
        else line = line + strlen(line);
    }


    Network snapshot = { rx, tx };
    return snapshot;
}



NetworkAverage parseNetworkUsage(Network snapshot2, Network snapshot1) {
    float rx = (float)snapshot2.rx - (float)snapshot1.rx;
    float tx = (float)snapshot2.tx - (float)snapshot1.tx;

    NetworkAverage n = {
        rx,
        tx
    };
    
    return n;
}