#include <criterion/criterion.h>

#include "parse.h"

Test(process, counts_running_processes_independently_of_display_limit) {
    Process processes[MAX_PROCESSES] = {0};
    Args args = {.sort = RAM, .processes = 0};

    unsigned process_count = getProcesses(processes, args);

    cr_assert_gt(process_count, 0);
    cr_assert_eq(processes[0].pid, 0);
}
