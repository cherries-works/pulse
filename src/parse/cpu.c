#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <time.h>

#include "utils.h"
#include "parse.h"
#include "http.h"

unsigned long parseCpuKey(char *buffer, char *target_key) {
    char *line = buffer;
    char *value = NULL;
    while(*line) {
        char *next = strchr(line, '\n');
        if(next) *next = '\0';

        char *space = strchr(line, ' ');
        if (space) {
            *space = '\0';
            trim(line);

            if (strcmp(line, target_key) == 0) {
                value = space + 1;

                trim(value);
                break;
            }
        }

        if (next == NULL) break;
        line = next + 1; 
    }

    if(value == NULL) return 1;
    return atoi(value);
}

struct Cpu getCpu(size_t size, char *buffer) {
    unsigned long user = 0;
    unsigned long nice = 0;
    unsigned long system = 0;
    unsigned long idle = 0;
    unsigned long iowait = 0;
    unsigned long irq = 0;
    unsigned long softirq = 0;
    unsigned long steal = 0;
    unsigned long guest = 0;
    unsigned long guest_nice = 0;

    unsigned number_position = 0;
    unsigned i = 0;
    unsigned number_index = 0;
    char number[16];
    while(i < size) {
        char character = buffer[i];
        if(character == '\n') break;

        if(character == ' ') {
            if(number_index != i) {
                switch (number_position) {
                    case 0:
                        user = atoi(number);
                        break;
                    case 1:
                        nice = atoi(number);
                        break;
                    case 2:
                        system = atoi(number);
                        break;
                    case 3:
                        idle = atoi(number);
                        break;
                    case 4:
                        iowait = atoi(number);
                        break;
                    case 5:
                        irq = atoi(number);
                        break;
                    case 6:
                        softirq = atoi(number);
                        break;
                    case 7:
                        steal = atoi(number);
                        break;
                    case 8:
                        guest = atoi(number);
                        break;
                    case 9:
                        guest_nice = atoi(number);
                        break;
                    
                    default:
                        break;
                }
                number_position++;
            } else i++;
            while(number_index > 0) {
                number[number_index] = '\0';
                number_index--;
            }

            i++;
            continue;
        }

        number[number_index] = character;
        number_index++;
        i++;
    }

    unsigned long user_time = user - guest;                             
    unsigned long nice_time = nice - guest_nice;                         
    unsigned long idle_all_time = idle + iowait;  
    unsigned long system_all_time = system + irq + softirq;
    unsigned long virt_all_time = guest + guest_nice;
    unsigned long total_time = user_time + nice_time + system_all_time + idle_all_time + steal + virt_all_time;

    unsigned long processes = parseCpuKey(buffer, "processes");
    struct Cpu snapshot = { idle_all_time, total_time, processes };
    return snapshot;
}

float parseCpuUsage(struct Cpu snapshot2, struct Cpu snapshot1) {
    float total_time_float = snapshot2.total == snapshot1.total ? (float)snapshot2.total : (float)snapshot2.total - (float)snapshot1.total;
    float idle_all_time_float = snapshot2.idle == snapshot1.idle ? (float)snapshot2.idle : (float)snapshot2.idle - (float)snapshot1.idle;

    float diff_total_idle = total_time_float - idle_all_time_float;
    float cpu_usage = diff_total_idle / total_time_float;
    cpu_usage *= 100;

    return cpu_usage;
}
