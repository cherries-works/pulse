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

void notifyDesktop(char *title, char *message, char *resource, float usage) {
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

void notifyDiscord(char *webhook, char *message, char *resource, float usage) {
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

void notifyCommand(char *command) {
    int result = system(command);
    if(result <= 0) return;
}

void notifyAlert(Config *config, char *resource, float usage) {
    if(config->commandNotify.enabled) {
        if(strlen(config->commandNotify.command) == 0) {
            _log(L_ERROR, "Command notification enabled, but not set (command).");
        } else {
            notifyCommand(config->commandNotify.command);
        }
    }
    if(config->discordNotify.enabled) {
        if(strlen(config->discordNotify.webhook) == 0 || strlen(config->discordNotify.message) == 0) {
            _log(L_ERROR, "Discord notification enabled, but not set (webhook or message).");
        } else {
            notifyDiscord(
                config->discordNotify.webhook,
                config->discordNotify.message,
                resource,
                usage
            );
        }
    }
    if(config->desktopNotify.enabled) {
        if(strlen(config->desktopNotify.title) == 0 || strlen(config->desktopNotify.message) == 0) {
            _log(L_ERROR, "Desktop notification enabled, but not set (title or message).");
        } else {
            notifyDesktop(
                config->desktopNotify.title,
                config->desktopNotify.message,
                resource,
                usage
            );
        }
    }
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
    float value,
    Alert *alert,
    char *name,
    int sleep,
    Config *config
) {
    float threshold = (float)alert->threshold / 100;

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
        notifyAlert(config, name, value);
        alert->current_cooldown = alert->cooldown;
    }

    alert->current_duration = 0;
}

void checkAlerts(Metrics *metrics, Args *args, Config *config) {
    if (!config->alerts.enabled) return;

    int sleep = (int)args->sleep;

    checkAlert(
        metrics->cpuUsage,
        &config->alerts.CPU,
        "CPU",
        sleep,
        config
    );

    checkAlert(
        metrics->ramUsage,
        &config->alerts.RAM,
        "RAM",
        sleep,
        config
    );

    checkAlert(
        metrics->diskUsage,
        &config->alerts.Disk,
        "Disk",
        sleep,
        config
    );
}