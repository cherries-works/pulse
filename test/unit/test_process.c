#include <criterion/criterion.h>

#include "parse.h"

Test(process, counts_running_processes_independently_of_display_limit) {
    Process processes[MAX_PROCESSES] = {0};
    CWArgs cwargs = {.sort = RAM, .processes = 0};
    CWInfo cwinfo = {};

    unsigned process_count = getProcesses(cwinfo, processes, cwargs);

    cr_assert_gt(process_count, 0);
    cr_assert_eq(processes[0].pid, 0);
}
