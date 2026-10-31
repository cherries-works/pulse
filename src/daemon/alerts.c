#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "utils.h"
#include "daemon.h"
#include "log.h"

void formatMessage(
    char *out, 
    size_t size,
    char *template,
    char *resource,
    float value
) {
    size_t value_string_size = 64;
    char value_string[value_string_size];
    snprintf(value_string, sizeof(value_string), "%.1f", value);

    while (*template && size > 1) {
        if (strncmp(template, "{resource}", 10) == 0) {
            size_t len = strlen(resource);
            memcpy(out, resource, len);
            out += len;
            size -= len;
            template += 10;
        }
        else if (strncmp(template, "{value}", 7) == 0) {
            size_t len = strlen(value_string);
            memcpy(out, value_string, len);
            out += len;
            size -= len;
            template += 7;
        }
        else {
            *out++ = *template++;
            size--;
        }
    }

    *out = '\0';
}

void notifyDesktop(CWInfo cwinfo, char *title, char *message, char *resource, float usage) {
    _log(cwinfo, L_INFO, "Notifying desktop.");

    size_t command_size = BUFFER_ONE_KB;
    char command[command_size];
    
    size_t out_message_size = BUFFER_ONE_KB;
    char out_message[out_message_size];
    formatMessage(out_message, out_message_size, message, resource, usage);

    snprintf(
        command,
        command_size,
        "notify-send "
        "-a \"Pulse\" "
        "-i \"./assets/favicon.png\" "
        "%s "
        "%s",
        title,
        out_message
    );

    int result = system(command);
    if(result <= 0) return;
}

void notifyDiscord(CWInfo cwinfo, char *webhook, char *message, char *resource, float usage) {
    _log(cwinfo, L_INFO, "Notifying discord.");

    size_t command_size = BUFFER_ONE_KB * 2;
    char command[command_size];

    size_t out_message_size = BUFFER_ONE_KB;
    char out_message[out_message_size];
    formatMessage(out_message, out_message_size, message, resource, usage);


    snprintf(
        command,
        sizeof(command),
        "curl -H \"Content-Type: application/json\" "
        "-X POST "
        "-d \"{\\\"content\\\":\\\"%s\\\"}\" "
        "\"%s\" > /dev/null 2>&1",
        out_message,
        webhook
    );

    int result = system(command);
    if(result <= 0) return;
}

void notifyCommand(CWInfo cwinfo, char *command) {
    _log(cwinfo, L_INFO, "Notifying command.");

    int result = system(command);
    if(result <= 0) return;
}

void notifyAlert(CWInfo cwinfo, CWConfig *cwconfig, char *resource, float usage) {
    _log(cwinfo, L_INFO, "Notifying with an alert.");

    if(cwconfig->commandNotify.enabled) {
        if(strlen(cwconfig->commandNotify.command) == 0) {
            _log(cwinfo, L_ERROR, "Command notification enabled, but not set (command).");
        } else {
            notifyCommand(cwinfo, cwconfig->commandNotify.command);
        }
    }
    if(cwconfig->discordNotify.enabled) {
        if(strlen(cwconfig->discordNotify.webhook) == 0 || strlen(cwconfig->discordNotify.message) == 0) {
            _log(cwinfo, L_ERROR, "Discord notification enabled, but not set (webhook or message).");
        } else {
            notifyDiscord(
                cwinfo,
                cwconfig->discordNotify.webhook,
                cwconfig->discordNotify.message,
                resource,
                usage
            );
        }
    }
    if(cwconfig->desktopNotify.enabled) {
        if(strlen(cwconfig->desktopNotify.title) == 0 || strlen(cwconfig->desktopNotify.message) == 0) {
            _log(cwinfo, L_ERROR, "Desktop notification enabled, but not set (title or message).");
        } else {
            notifyDesktop(
                cwinfo,
                cwconfig->desktopNotify.title,
                cwconfig->desktopNotify.message,
                resource,
                usage
            );
        }
    }

    _log(cwinfo, L_INFO, "Done notifying.");
}

bool checkCondition(float value, float threshold, Operator op) {
    switch (op) {
        case E:  return value == threshold;
        case GE: return value >= threshold;
        case LE: return value <= threshold;
        case L:  return value < threshold;
        case G:  return value > threshold;
        case NE: return value != threshold;
        default: return false;
    }
}

static void checkAlert(
    CWInfo cwinfo,
    float value,
    Alert *alert,
    char *name,
    int sleep,
    CWConfig *cwconfig
) {
    float threshold = (float)alert->threshold;

    if (!checkCondition(value, threshold, alert->op)) {
        alert->current_duration = 0;
        alert->current_cooldown = 0;
        return;
    }

    alert->current_duration += sleep;

    if (alert->current_cooldown > 0) {
        // subtract the sleep WITH the cooldown
        // otherwise, the cooldown counter
        // never goes down unless the duration
        // counter is hit (duration * cooldown)
        alert->current_cooldown -= sleep;

        if (alert->current_cooldown < 0) {
            alert->current_cooldown = 0;
        }
    }

    if (alert->current_duration < alert->duration) return;

    if (alert->current_cooldown == 0) {
        notifyAlert(cwinfo, cwconfig, name, value);
        alert->current_cooldown = alert->cooldown;
    }

    alert->current_duration = 0;
}

void checkAlerts(CWInfo cwinfo, Metrics *metrics, CWArgs *cwargs, CWConfig *cwconfig) {
    if (!cwconfig->alerts.enabled) return;

    int sleep = (int)cwargs->sleep;

    checkAlert(
        cwinfo,
        metrics->cpuUsage,
        &cwconfig->alerts.CPU,
        "CPU",
        sleep,
        cwconfig
    );

    checkAlert(
        cwinfo,
        metrics->ramUsage,
        &cwconfig->alerts.RAM,
        "RAM",
        sleep,
        cwconfig
    );

    checkAlert(
        cwinfo,
        metrics->diskUsage,
        &cwconfig->alerts.Disk,
        "Disk",
        sleep,
        cwconfig
    );
}