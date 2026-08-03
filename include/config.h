#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

typedef enum {
    G,
    E,
    L,
    GE,
    LE,
    NE,
} Operator;

typedef struct {
    int threshold;
    Operator op;
    int duration;
    int current_duration;
} Alert;

typedef struct {
    bool enabled;

    Alert CPU;
    Alert RAM;
    Alert Disk;
} Alerts;

typedef struct {
    bool enabled;
    char title[256];
    char message[256];
} DesktopNotification;

typedef struct {
    bool enabled;
    char webhook[256];
    char message[256];
} DiscordNotification;

typedef struct {
    bool enabled;
    char command[256];
} CommandNotification;

typedef struct {
    Alerts alerts;

    DesktopNotification desktopNotify;
    DiscordNotification discordNotify;
    CommandNotification commandNotify;
} Config;

extern Config parseToml();

#endif