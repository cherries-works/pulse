#include <stdio.h>
#include <criterion/criterion.h>

#include "parse.h"

Test(network, parses_network_statistics) {
    char buffer[] =
        "Inter-|   Receive                                                |  Transmit\n"
        " face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed\n"
        "  lo: 1000     10      0    0    0     0          0         0       2000     20      0    0    0     0       0          0\n"
        "eth0: 3000     30      0    0    0     0          0         0       4000     40      0    0    0     0       0          0\n";

    Network network = getNetwork(sizeof(buffer), buffer);

    cr_assert_eq(network.rx, 1000);
    cr_assert_eq(network.tx, 2000);
}

Test(network, handles_zero_values) {
    char buffer[] =
        "Inter-|   Receive                                                |  Transmit\n"
        " face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed\n"
        "  lo: 0        0       0    0    0     0          0         0       0        0       0    0    0     0       0          0\n";

    Network network = getNetwork(sizeof(buffer), buffer);

    cr_assert_eq(network.rx, 0);
    cr_assert_eq(network.tx, 0);
}

Test(network, handles_large_values) {
    char buffer[] =
        "Inter-|   Receive                                                |  Transmit\n"
        " face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed\n"
        "eth0: 4294967295 1 0 0 0 0 0 0 4294967295 1 0 0 0 0 0 0\n";

    Network network = getNetwork(sizeof(buffer), buffer);

    cr_assert_eq(network.rx, 4294967295);
    cr_assert_eq(network.tx, 4294967295);
}