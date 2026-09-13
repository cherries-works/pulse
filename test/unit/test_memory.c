#include <criterion/criterion.h>

#include "parse.h"

Test(memory, parses_memtotal) {
    char buffer[] =
        "MemTotal:       16384000 kB\n"
        "MemFree:         2048000 kB\n"
        "MemAvailable:    8192000 kB\n";


    Memory memory = getMemory(sizeof(buffer), buffer);

    cr_assert_eq(memory.total, 16384000);
    cr_assert_eq(memory.available, 8192000);
}


Test(memory, returns_one_for_missing_key) {
    char buffer[] =
        "MemTotal:       16384000 kB\n"
        "MemFree:         2048000 kB\n";

    Memory memory = getMemory(sizeof(buffer), buffer);

    cr_assert_eq(memory.available, 0);
}

Test(memory, handles_variable_whitespace) {
    char buffer[] =
        "MemTotal:  16384000 kB\n"
        "MemAvailable:       8192000 kB\n";

    Memory memory = getMemory(sizeof(buffer), buffer);


    cr_assert_eq(memory.total, 16384000);
    cr_assert_eq(memory.available, 8192000);
}
