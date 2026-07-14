#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "utils.h"
#include "parse.h"

Info getInfo() {
    Kernel kernel = {
        .machine = "",
        .release = "",
        .sysname = ""
    };

    Info info = {
        .cpu_model = "",
        .hostname = "",
        .os = "",
        .desktop = "",
        .session = "",
        .kernel = kernel,
        .cores = 0
    };
    
    strcpy(info.desktop, getenv("XDG_CURRENT_DESKTOP"));
    strcpy(info.session, getenv("XDG_SESSION_TYPE"));

    struct utsname u;
    uname(&u);

    strcpy(info.kernel.release, u.release);
    strcpy(info.kernel.machine, u.machine);
    strcpy(info.kernel.sysname, u.sysname);

    int ghn = gethostname(info.hostname, sizeof(info.hostname));
    if(ghn < 0) {
        printf("Reading hostname failed.");
    }

    info.cores = (unsigned)sysconf(_SC_NPROCESSORS_ONLN);

    size_t cpu_buffer_size = BUFFER_ONE_KB * 6;
    char cpu_buffer[cpu_buffer_size];
    readFile(PROC_CPU_FILE, cpu_buffer_size, cpu_buffer);

    char *cpu = cpu_buffer;
    char *n = strchr(cpu, '\n');
    while(n != NULL) {
        *n = '\0';

        char *dots = strchr(cpu, ':');
        if(dots == NULL) {
            cpu = n + 1;
            n = strchr(cpu, '\n');
            continue;
        }
        *dots = '\0';

        size_t len = strlen(cpu);
        cpu[len - 1] = '\0';
        if(strcmp("model name", cpu) != 0) {
            cpu = n + 1;
            n = strchr(cpu, '\n');
            continue;
        }

        cpu = dots + 1;
        cpu += 1; // skip space in between :
        strcpy(info.cpu_model, cpu);
    }

    size_t etc_buffer_size = BUFFER_ONE_KB * 6;
    char etc_buffer[etc_buffer_size];
    readFile(ETC_OS_RELEASE, etc_buffer_size, etc_buffer);

    char *etc = etc_buffer;
    n = strchr(etc, '\n');
    while(n != NULL) {
        *n = '\0';

        char *eq = strchr(etc, '=');
        if(eq == NULL) {
            etc = n + 1;
            n = strchr(etc, '\n');
            continue;
        }
        *eq = '\0';

        size_t len = strlen(etc);
        if(strcmp("PRETTY_NAME", etc) != 0) {
            etc = n + 1;
            n = strchr(etc, '\n');
            continue;
        }

        etc = eq + 1;
        etc += 1; // skip the quote start
        len = strlen(etc);
        etc[len - 1] = '\0'; // skip the last quote

        strcpy(info.os, etc);
    }

    return info;
}
