#include <stdio.h>
#include <criterion/criterion.h>

#include "parse.h"


Test(disk, parses_disk_statistics) {
    char buffer[] = "nvme0n1 100 20 300 40 500 60 700 80 90 1000 110\n";

    Disk disk = getDisk(sizeof(buffer), buffer);

    cr_assert_eq(disk.read, 60);
    cr_assert_eq(disk.write, 1000);
}


Test(disk, handles_zero_values) {
    char buffer[] =
        "nvme0n1 0 0 0 0 0 0 0 0 0 0 0\n";

    Disk disk = getDisk(sizeof(buffer), buffer);

    cr_assert_eq(disk.read, 0);
    cr_assert_eq(disk.write, 0);
}


Test(disk, parses_large_values) {
    char buffer[] =
        "nvme0n1 "
        "4294967295 "
        "4294967295 "
        "4294967295 "
        "4294967295 "
        "4294967295 "
        "4294967295 "
        "4294967295 "
        "4294967295 "
        "4294967295 "
        "4294967295 "
        "4294967295\n";

    Disk disk = getDisk(sizeof(buffer), buffer);
    
    cr_assert_eq(disk.read, 4294967295);
    cr_assert_eq(disk.write, 4294967295);
}


Test(disk, gets_filesystem_information) {
    char buffer[] =
        "nvme0n1 100 20 300 40 500 60 700 80 90 1000 110\n";

    Disk disk = getDisk(sizeof(buffer), buffer);

    cr_assert_gt(disk.total, 0);
    cr_assert_gt(disk.available, 0);
    cr_assert(disk.available <= disk.total);
}