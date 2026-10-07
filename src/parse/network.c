#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "parse.h"
#include "log.h"

Network getNetwork(size_t size, char *buffer) {
    _log(L_INFO, "Getting network info.");

    uint64_t rx = 0;
    uint64_t tx = 0;

    char *line = buffer;
    for (int i = 0; i < 2; i++) {
        char *next = strchr(line, '\n');

        if (next) {
            line = next + 1;
        } else {
            Network network = { 0, 0 };
            return network;
        }
    }

    // Inter-|   Receive           |  Transmit                          \n <--- first strchr
    // face  |bytes  ..  multicast|bytes    packets .... compressed     \n <--- second strchr
    //                                                                  So we start at the start of lo
    // lo:   38914   ..  38914     307    0    ....                0
    // enp2s0:    0 ....         0        0       ....               0
    // wlp0s20f3: 183282384 ....         0 18357363   ....       0          0
    int KEYS_AMOUNT = 8;
    while (*line) {
        char *colon = strchr(line, ':');
        if (!colon) break;

        line = colon + 1;

        for (int i = 0; i < KEYS_AMOUNT; i++) {
            while (*line == ' ') line++;

            char *next = strchr(line, ' ');

            if (next) *next = '\0';

            if (i == 0) rx += strtoul(line, NULL, 10);
            if (next) {
                line = next + 1;
            } else {
                break;
            }
        }

        for (int i = 0; i < KEYS_AMOUNT; i++) {
            while (*line == ' ') line++;
            char *next = strchr(line, ' ');

            if (next) *next = '\0';

            if (i == 0) tx += strtoul(line, NULL, 10);

            if (next) {
                line = next + 1;
            } else {
                break;
            }
        }

        // Move to the next interface.
        char *next = strchr(line, '\n');

        if (next) {
            line = next + 1;
        } else {
            break;
        }
    }

    _log(L_INFO, "(Done) Getting network info.");

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
