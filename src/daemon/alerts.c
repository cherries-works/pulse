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
        "\"%s\" "
        "\"%s\"",
        title,
        out_message
    );

    system(command);
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

    system(command);
}

void notifyCommand(char *command) {
    system(command);
}

void notifyAlert(Config config, char *resource, float usage) {
    if(config.commandNotify.enabled) {
        notifyCommand(config.commandNotify.command);
    }
    if(config.discordNotify.enabled) {
        notifyDiscord(
            config.discordNotify.webhook,
            config.discordNotify.message,
            resource,
            usage
        );
    }
    if(config.desktopNotify.enabled) {
        notifyDesktop(
            config.desktopNotify.title,
            config.desktopNotify.message,
            resource,
            usage
        );
    }
}

void checkAlerts(Metrics metrics, Args args, Config *config) {
    if(!config->alerts.enabled) return;

    switch (config->alerts.CPU.op) {
        case E: {
            if(metrics.cpuUsage == ((float)(config->alerts.CPU.threshold) / 100)) {
                config->alerts.CPU.current_duration += (int)args.sleep;
            } else {
                config->alerts.CPU.current_duration = 0;
            }
            break;
        }
        
        case GE: {
            if(metrics.cpuUsage <= ((float)(config->alerts.CPU.threshold) / 100)) {
                config->alerts.CPU.current_duration += (int)args.sleep;
            } else {
                config->alerts.CPU.current_duration = 0;
            }
            break;
        }
        
        case LE: {
            if(metrics.cpuUsage >= ((float)(config->alerts.CPU.threshold) / 100)) {
                config->alerts.CPU.current_duration += (int)args.sleep;
            } else {
                config->alerts.CPU.current_duration = 0;
            }
            break;
        }
        
        case L: {
            if(metrics.cpuUsage > ((float)(config->alerts.CPU.threshold) / 100)) {
                config->alerts.CPU.current_duration += (int)args.sleep;
            } else {
                config->alerts.CPU.current_duration = 0;
            }
            break;
        }
        
        case G: {
            if(metrics.cpuUsage < ((float)(config->alerts.CPU.threshold) / 100)) {
                config->alerts.CPU.current_duration += (int)args.sleep;
            } else {
                config->alerts.CPU.current_duration = 0;
            }
            break;
        }

        case NE: {
            if(metrics.cpuUsage != ((float)(config->alerts.CPU.threshold) / 100)) {
                config->alerts.CPU.current_duration += (int)args.sleep;
            } else {
                config->alerts.CPU.current_duration = 0;
            }
            break;
        }

        default: {
            break;
        }
    }

    switch (config->alerts.RAM.op) {
        case E: {
            if(metrics.ramUsage == ((float)(config->alerts.RAM.threshold) / 100)) {
                config->alerts.RAM.current_duration += (int)args.sleep;
            } else {
                config->alerts.RAM.current_duration = 0;
            }
            break;
        }

        case GE: {
            if(metrics.ramUsage <= ((float)(config->alerts.RAM.threshold) / 100)) {
                config->alerts.RAM.current_duration += (int)args.sleep;
            } else {
                config->alerts.RAM.current_duration = 0;
            }
            break;
        }

        case LE: {
            if(metrics.ramUsage >= ((float)(config->alerts.RAM.threshold) / 100)) {
                config->alerts.RAM.current_duration += (int)args.sleep;
            } else {
                config->alerts.RAM.current_duration = 0;
            }
            break;
        }

        case L: {
            if(metrics.ramUsage > ((float)(config->alerts.RAM.threshold) / 100)) {
                config->alerts.RAM.current_duration += (int)args.sleep;
            } else {
                config->alerts.RAM.current_duration = 0;
            }
            break;
        }

        case G: {
            if(metrics.ramUsage < ((float)(config->alerts.RAM.threshold) / 100)) {
                config->alerts.RAM.current_duration += (int)args.sleep;
            } else {
                config->alerts.RAM.current_duration = 0;
            }
            break;
        }

        case NE: {
            if(metrics.ramUsage != ((float)(config->alerts.RAM.threshold) / 100)) {
                config->alerts.RAM.current_duration += (int)args.sleep;
            } else {
                config->alerts.RAM.current_duration = 0;
            }
            break;
        }

        default: {
            break;
        }
    }
 
    switch (config->alerts.Disk.op) {
        case E: {
            if(metrics.diskUsage == ((float)(config->alerts.Disk.threshold) / 100)) {
                config->alerts.Disk.current_duration += (int)args.sleep;
            } else {
                config->alerts.Disk.current_duration = 0;
            }
            break;
        }

        case GE: {
            if(metrics.diskUsage <= ((float)(config->alerts.Disk.threshold) / 100)) {
                config->alerts.Disk.current_duration += (int)args.sleep;
            } else {
                config->alerts.Disk.current_duration = 0;
            }
            break;
        }

        case LE: {
            if(metrics.diskUsage >= ((float)(config->alerts.Disk.threshold) / 100)) {
                config->alerts.Disk.current_duration += (int)args.sleep;
            } else {
                config->alerts.Disk.current_duration = 0;
            }
            break;
        }

        case L: {
            if(metrics.diskUsage > ((float)(config->alerts.Disk.threshold) / 100)) {
                config->alerts.Disk.current_duration += (int)args.sleep;
            } else {
                config->alerts.Disk.current_duration = 0;
            }
            break;
        }

        case G: {
            if(metrics.diskUsage < ((float)(config->alerts.Disk.threshold) / 100)) {
                config->alerts.Disk.current_duration += (int)args.sleep;
            } else {
                config->alerts.Disk.current_duration = 0;
            }
            break;
        }

        case NE: {
            if(metrics.diskUsage != ((float)(config->alerts.Disk.threshold) / 100)) {
                config->alerts.Disk.current_duration += (int)args.sleep;
            } else {
                config->alerts.Disk.current_duration = 0;
            }
            break;
        }

        default: {
            break;
        }
    }

    if(config->alerts.CPU.duration <= config->alerts.CPU.current_duration) {
        notifyAlert(*config, "CPU", metrics.cpuUsage);
        config->alerts.CPU.current_duration = 0;
    }
    if(config->alerts.RAM.duration <= config->alerts.RAM.current_duration) {
        notifyAlert(*config, "RAM", metrics.ramUsage);
        config->alerts.RAM.current_duration = 0;
    }
    if(config->alerts.Disk.duration <= config->alerts.Disk.current_duration) {
        notifyAlert(*config, "Disk", metrics.diskUsage);
        config->alerts.Disk.current_duration = 0;
    }
}
