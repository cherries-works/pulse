#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "utils.h"
#include "log.h"
#include "config.h"
#include "setup.h"
#include "args.h"

void config(CWArgs cwargs, CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running cwconfig() -> command cwconfig"
    );

    char path[BUFFER_ONE_KB];
    snprintf(path, BUFFER_ONE_KB, "%s/cwconfig.toml", cwinfo.pulse);
   
    // enum to string conversion
    const char* operatorToString(Operator op) {
        switch (op) {
            case G:  return ">";
            case E:  return "==";
            case L:  return "<";
            case GE: return ">=";
            case LE: return "<=";
            case NE: return "!=";
            default: return ">";
        }
    }

    if(cwargs.current) {
        CWConfig _cwconfig = parseToml(cwinfo);

        if(cwargs.json) {
            printf(
                "{"
                "\"alerts\":{"
                "\"cpu\":{\"op\":\"%s\",\"threshold\":%d,\"duration\":%d},"
                "\"ram\":{\"op\":\"%s\",\"threshold\":%d,\"duration\":%d},"
                "\"disk\":{\"op\":\"%s\",\"threshold\":%d,\"duration\":%d}"
                "},"
                "\"notifications\":{\"desktop\":%s,\"discord\":%s,\"command\":%s}"
                "}\n",
                operatorToString(_cwconfig.alerts.CPU.op), _cwconfig.alerts.CPU.threshold, _cwconfig.alerts.CPU.duration,
                operatorToString(_cwconfig.alerts.RAM.op), _cwconfig.alerts.RAM.threshold, _cwconfig.alerts.RAM.duration,
                operatorToString(_cwconfig.alerts.Disk.op), _cwconfig.alerts.Disk.threshold, _cwconfig.alerts.Disk.duration,
                _cwconfig.desktopNotify.enabled ? "true" : "false",
                _cwconfig.discordNotify.enabled ? "true" : "false",
                _cwconfig.commandNotify.enabled ? "true" : "false"
            );
            return;
        }

        printf("Alerts\n");
        printf("CPU:  %-2s %d%% for %ds\n", operatorToString(_cwconfig.alerts.CPU.op), _cwconfig.alerts.CPU.threshold, _cwconfig.alerts.CPU.duration);
        printf("RAM:  %-2s %d%% for %ds\n", operatorToString(_cwconfig.alerts.RAM.op), _cwconfig.alerts.RAM.threshold, _cwconfig.alerts.RAM.duration);
        printf("Disk: %-2s %d%% for %ds\n", operatorToString(_cwconfig.alerts.Disk.op), _cwconfig.alerts.Disk.threshold, _cwconfig.alerts.Disk.duration);

        printf("\nNotifications\n");
        printf("Desktop:  %s\n", _cwconfig.desktopNotify.enabled ? "enabled" : "disabled");
        printf("Discord:  %s\n", _cwconfig.discordNotify.enabled ? "enabled" : "disabled");
        printf("Command:  %s\n", _cwconfig.commandNotify.enabled ? "enabled" : "disabled");
        return;
    }

    if(cwargs.reset) {
        remove(path);
        int fd = creat(path, 0644);
        ssize_t result = write(fd, CHERRIES_DEFAULT_TOML, strlen(CHERRIES_DEFAULT_TOML));
        if(result <= 0) {
            _log(cwinfo, L_ERROR, "Failed to reset cwconfig file.");
        }
        return;
    }
    char command[BUFFER_ONE_KB];
    snprintf(command, BUFFER_ONE_KB, "nano %s", path);

    int result = system(command);
    if(result <= 0) return;
}
