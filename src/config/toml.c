#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "config.h"
#include "utils.h"

int isDigit(char ch) {
    return (ch >= '0' && ch <= '9') ? 1 : 0;
}

int parseWait(char *wait) {
    size_t size = strlen(wait);

    int total_time = 0;
    int saved_time = 0;
    for(size_t i = 0; i < size; i++) {
        char w = wait[i];
        if(w == ' ') continue;
        if(isDigit(w) == 1) {
            if(saved_time > 0) {
                saved_time *= 10;
            }

            saved_time += (int)(w - '0');
        } else {
            if(w == 's') {
                total_time += saved_time;
            } else if(w == 'm') {
                total_time += saved_time * 60;
            } else if(w == 'h') {
                total_time += saved_time * 60 * 60;
            } else if(w == 'd') {
                total_time += saved_time * 60 * 60 * 24;
            }

            saved_time = 0;
            while(wait[i] != EOF && wait[i] != '\0' && wait[i] != ' ' && isDigit(wait[i]) != 1) {
                i++;
            }
        }
    }

    return total_time;
}

Config parseToml() {
    Config config = {};

    char *home = getenv("HOME");
    if(home == NULL) return config;

    char path[BUFFER_ONE_KB];
    snprintf(path, BUFFER_ONE_KB, "%s/%s/config.toml", home, R_CHERRIES_FOLDER_PULSE);

    char conf[BUFFER_ONE_KB];
    size_t n = readFile(path, BUFFER_ONE_KB, conf);
    conf[n] = '\0';

    char *buf = conf;
    char *newline = strchr(buf, '\n');
    
    char *current_section = "";
    char *current_target = "";

    while(newline != NULL) {
        *newline = '\0';
        if(strlen(buf) == 0) {
            buf = newline + 1;
            newline = strchr(buf, '\n');
            continue;
        }

        if(strcmp(buf, "[alerts]") == 0) {
            current_section = "alerts";
            buf = newline + 1;
            newline = strchr(buf, '\n');
            continue;
        }

        if(strcmp(buf, "[notifications]") == 0) {
            current_section = "notifications";
            buf = newline + 1;
            newline = strchr(buf, '\n');
            continue;
        }

        if(strcmp(current_section, "alerts") == 0) {
            if(strcmp(buf, "[alerts.cpu]") == 0) {
                current_target = "cpu";
                buf = newline + 1;
                newline = strchr(buf, '\n');
                continue;
            }

            if(strcmp(buf, "[alerts.ram]") == 0) {
                current_target = "ram";
                buf = newline + 1;
                newline = strchr(buf, '\n');
                continue;
            }

            if(strcmp(buf, "[alerts.disk]") == 0) {
                current_target = "disk";
                buf = newline + 1;
                newline = strchr(buf, '\n');
                continue;
            }

            char *eq = strchr(buf, '=');
            if(eq != NULL) *eq = '\0';
            trim(buf);
            if(strlen(current_target) == 0) {
                if(startsWith(buf, strlen(buf), "enabled", strlen("enabled"))) {
                    buf = eq + 1;
                    trim(buf);
                    if(strcmp(buf, "true") == 0) config.alerts.enabled = true;
                    else config.alerts.enabled = false;
                }
            }

            if(startsWith(buf, strlen(buf), "threshold", strlen("threshold"))) {
                buf = eq + 1;
                trim(buf);
                int threshold = atoi(buf);

                if(strcmp(current_target, "ram") == 0) config.alerts.RAM.threshold = threshold;
                if(strcmp(current_target, "cpu") == 0) config.alerts.CPU.threshold = threshold;
                if(strcmp(current_target, "disk") == 0) config.alerts.Disk.threshold = threshold;
            }

            if(startsWith(buf, strlen(buf), "duration", strlen("duration"))) {
                buf = eq + 1;
                trim(buf);
                
                int duration = parseWait(buf);

                if(strcmp(current_target, "ram") == 0) config.alerts.RAM.duration = duration;
                if(strcmp(current_target, "cpu") == 0) config.alerts.CPU.duration = duration;
                if(strcmp(current_target, "disk") == 0) config.alerts.Disk.duration = duration;
            }

            if(startsWith(buf, strlen(buf), "operator", strlen("operator"))) {
                buf = eq + 1;
                trim(buf);

                Operator op = G;

                if(strcmp(buf, "==") == 0) op = E;
                if(strcmp(buf, ">=") == 0) op = GE;
                if(strcmp(buf, "!=") == 0) op = NE;
                if(strcmp(buf, "<") == 0) op = L;
                if(strcmp(buf, "<=") == 0) op = LE;

                if(strcmp(current_target, "ram") == 0) config.alerts.RAM.op = op;
                if(strcmp(current_target, "cpu") == 0) config.alerts.CPU.op = op;
                if(strcmp(current_target, "disk") == 0) config.alerts.Disk.op = op;
            }
        }

        if(strcmp(current_section, "notifications") == 0) {
            if(strcmp(buf, "[notifications.desktop]") == 0) {
                current_target = "desktop";
                buf = newline + 1;
                newline = strchr(buf, '\n');
                continue;
            }

            if(strcmp(buf, "[notifications.discord]") == 0) {
                current_target = "discord";
                buf = newline + 1;
                newline = strchr(buf, '\n');
                continue;
            }

            if(strcmp(buf, "[notifications.command]") == 0) {
                current_target = "command";
                buf = newline + 1;
                newline = strchr(buf, '\n');
                continue;
            }


            char *eq = strchr(buf, '=');
            if(eq != NULL) *eq = '\0';
            trim(buf);

            if(startsWith(buf, strlen(buf), "enabled", strlen("enabled"))) {
                buf = eq + 1;
                trim(buf);
                
                bool enabled = true;
                if(strcmp(buf, "false") == 0) enabled = false;

                if(strcmp(current_target, "discord") == 0) config.discordNotify.enabled = enabled;
                if(strcmp(current_target, "desktop") == 0) config.desktopNotify.enabled = enabled;
                if(strcmp(current_target, "command") == 0) config.commandNotify.enabled = enabled;
            }

            if(startsWith(buf, strlen(buf), "title", strlen("title"))) {
                buf = eq + 1;
                trim(buf);

                if(strcmp(current_target, "desktop") == 0) {
                    strcpy(config.desktopNotify.title, buf);
                }
            }

            if(startsWith(buf, strlen(buf), "message", strlen("message"))) {
                buf = eq + 1;
                trim(buf);

                if(strcmp(current_target, "desktop") == 0) {
                    strcpy(config.desktopNotify.message, buf);
                }
                if(strcmp(current_target, "discord") == 0) {
                    strcpy(config.discordNotify.message, buf);
                }
            }

            if(startsWith(buf, strlen(buf), "webhook", strlen("webhook"))) {
                buf = eq + 1;
                trim(buf);

                if(strcmp(current_target, "discord") == 0) {
                    strcpy(config.discordNotify.webhook, buf);
                }
            }

            if(startsWith(buf, strlen(buf), "command", strlen("command"))) {
                buf = eq + 1;
                trim(buf);

                if(strcmp(current_target, "command") == 0) {
                    strcpy(config.commandNotify.command, buf);
                }
            }
        }

        buf = newline + 1;
        newline = strchr(buf, '\n');
    }

    return config;
}
