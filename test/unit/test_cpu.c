#include <criterion/criterion.h>

#include "parse.h"

Test(cpu, parses_proc_stat) {
    char buffer[] =
        "cpu  100 20 30 400 50 10 5 5 2 1\n"
        "processes 12345\n";

    size_t size = sizeof(buffer) - 1;
    Cpu cpu = getCpu(size, buffer);

    cr_assert_eq(cpu.idle, 450);
    cr_assert_eq(cpu.total, 620);
    cr_assert_eq(cpu.processes, 12345);
}


Test(cpu, handles_zero_values) {
    char buffer[] =
        "cpu  0 0 0 0 0 0 0 0 0 0\n"
        "processes 0\n";

    size_t size = sizeof(buffer) - 1;

    Cpu cpu = getCpu(size, buffer);

    cr_assert_eq(cpu.idle, 0);
    cr_assert_eq(cpu.total, 0);
    cr_assert_eq(cpu.processes, 0);
}


Test(cpu, handles_large_values) {
    char buffer[] =
        "cpu  4294967295 100 200 300 400 500 600 700 800 900\n"
        "processes 999999\n";

    size_t size = sizeof(buffer) - 1;

    Cpu cpu = getCpu(size, buffer);

    cr_assert_eq(cpu.processes, 999999);
    cr_assert(cpu.total > 0);
}


Test(cpu, ignores_cpu_core_lines) {
    char buffer[] =
        "cpu  100 20 30 400 50 10 5 5 2 1\n"
        "cpu0 50 10 15 200 20 5 2 2 1 0\n"
        "processes 12345\n";

    size_t size = sizeof(buffer) - 1;

    Cpu cpu = getCpu(size, buffer);

    /*
     * getCpu() should only parse the aggregate "cpu" line.
     */
    cr_assert_eq(cpu.idle, 450);
    cr_assert_eq(cpu.processes, 12345);
}