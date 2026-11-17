#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"
#include "daemon.h"
#include "log.h"
#include "app.h"
#include "setup.h"

void prune(CWArgs cwargs, CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running prune() -> command prune"
    );

    Prune prune = cwargs.prune;
    uint8_t keep = cwargs.keep;
    uint64_t until = unformatTime(cwargs.until);

    size_t path_size = BUFFER_ONE_KB;
    uint16_t history_deleted = 0;
    uint16_t logs_deleted = 0;

    if(prune == HISTORY || prune == ALL) {
        char path[path_size];
        snprintf(path, path_size, "%s/history", cwinfo.pulse);

        uint16_t path_entries_count = countDir(path);
        uint16_t path_entries_deleted = 0;

        char entry_path[path_size];

        struct dirent *entry;
        DIR *dir = opendir(path);

        while((entry = readdir(dir)) != NULL) {
            if(path_entries_count - path_entries_deleted <= keep) break;

            char *entry_name = entry->d_name;
            if(strcmp(entry_name, ".") == 0) continue;
            if(strcmp(entry_name, "..") == 0) continue;

            snprintf(entry_path, path_size, "%s/history/%s", cwinfo.pulse, entry_name);

            uint64_t _entry_time = unformatTime(entry_name);
            if(_entry_time < until) {
                cleanDir(entry_path);
                path_entries_deleted++;
            }
        }

        closedir(dir);
        history_deleted = path_entries_deleted;
    }

    if(prune == LOGS || prune == ALL) {
        char path[path_size];
        snprintf(path, path_size, "%s/logs", cwinfo.pulse);

        uint16_t path_entries_count = countDir(path);
        uint16_t path_entries_deleted = 0;

        char entry_path[path_size];

        struct dirent *entry;
        DIR *dir = opendir(path);
        
        while((entry = readdir(dir)) != NULL) {
            if(path_entries_count - path_entries_deleted <= keep) break;

            char *entry_name = entry->d_name;
            if(strcmp(entry_name, ".") == 0) continue;
            if(strcmp(entry_name, "..") == 0) continue;

            snprintf(entry_path, path_size, "%s/logs/%s", cwinfo.pulse, entry_name);
            uint64_t _entry_time = unformatTime(entry_name);

            if(_entry_time < until) {
                remove(entry_path);
                path_entries_deleted++;
            }
        }

        closedir(dir);
        logs_deleted = path_entries_deleted;
    }

    if(cwargs.json) {
        printf(
            "{"
            "\"timestamp\":%" PRIu64 ","
            "\"history\":%u,"
            "\"logs\":%u"
            "}\n",
            time(NULL),
            history_deleted,
            logs_deleted
        );
    }
}
