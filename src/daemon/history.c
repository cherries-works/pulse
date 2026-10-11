// This file includes the history retention mechanism for Pulse.
//
// Every hour a history file is written, after 24 hours have passed
// for a history file it gets archived and stored within the /pulse/archived
// directory (under its fitting date stamp). The file is compressed
// with gz, and then stored for upwards of 30 days, after which, they
// are completely deleted.
//
//
// > ARCHIVE WHEN 24 HOURS PASSED SINCE CREATION.
// > DELETE WHEN 30 DAYS PASSED SINCE ARCHIVING.
//
// Compression algorithm used: gz
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <inttypes.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <zlib.h>
#include <errno.h>

#include "utils.h"
#include "daemon.h"
#include "log.h"
#include "setup.h"

void _archiveHistory(CWInfo cwinfo, char *type) {
    size_t time_buffer_size = BUFFER_ONE_KB * 8;
    char time_buffer[time_buffer_size];
    formatTime(cwinfo.started_at, time_buffer, time_buffer_size);

    time_t _time = time(NULL);
    time_t current_interval = _time - (_time % HISTORY_INTERVAL);

    size_t history_path_size = BUFFER_ONE_KB;
    char history_path[history_path_size];
    snprintf(
        history_path, history_path_size,
        "%s/history/%s/%s",
        cwinfo.pulse,
        time_buffer,
        type
    );

    DIR *dir = opendir(history_path);
    if (dir == NULL) {
        _log(cwinfo, L_ERROR, "Opening history directory failed.");
        return;
    }

    struct dirent *entry;

    while((entry = readdir(dir)) != NULL) {
        char *entry_name = entry->d_name;
        if(strcmp(entry_name, ".") == 0) continue;
        if(strcmp(entry_name, "..") == 0) continue;

        unsigned long timestamp = strtoul(entry_name, NULL, 10);
        if((unsigned long)current_interval - timestamp >= ARCHIVE_INTERVAL) {
            size_t full_entry_path_size = BUFFER_ONE_KB;
            char full_entry_path[full_entry_path_size];

            snprintf(
                full_entry_path, full_entry_path_size,
                "%s/%s",
                history_path,
                entry_name
            );

            FILE *hf = fopen(full_entry_path, "r");
            if (hf == NULL) {
                char message[BUFFER_ONE_KB];

                snprintf(
                    message, sizeof(message),
                    "Opening history file '%s' failed: %s",
                    full_entry_path,
                    strerror(errno)
                );

                _log(cwinfo, L_ERROR, message);
                continue;
            }

            size_t file_buffer_size = BUFFER_ONE_KB * 8;
            char file_buffer[file_buffer_size];
            size_t file_size = fread(file_buffer, 1, file_buffer_size - 1, hf);

            size_t archived_path_size = BUFFER_ONE_KB;
            char archived_path[archived_path_size];
            snprintf(
                archived_path, archived_path_size,
                "%s/archived/%s/%s/%s.gz",
                cwinfo.pulse,
                time_buffer,
                type,
                entry_name
            );

            gzFile file = gzopen(archived_path, "wb");
            int _gzwrite = gzwrite(file, file_buffer, (unsigned)file_size);
            if(_gzwrite < 0) {
                _log(cwinfo, L_ERROR, "Writing gzip file failed for archiving.");
                fclose(hf);
                continue;
            }

            gzclose(file);
            fclose(hf);
            int _remove = remove(full_entry_path); // delete the file that was archived
            if(_remove == -1) {
                _log(cwinfo, L_ERROR, "Removing entry failed while archiving.");
            }
            continue;
        }
    }

    closedir(dir);
}

void _deleteHistoryArchives(CWInfo cwinfo, char *type) {
    size_t time_buffer_size = BUFFER_ONE_KB * 8;
    char time_buffer[time_buffer_size];
    formatTime(cwinfo.started_at, time_buffer, time_buffer_size);

    time_t _time = time(NULL);
    time_t current_interval = _time - (_time % HISTORY_INTERVAL);

    size_t archived_path_size = BUFFER_ONE_KB;
    char archived_path[archived_path_size];
    snprintf(
        archived_path, archived_path_size,
        "%s/archived/%s/%s",
        cwinfo.pulse,
        time_buffer,
        type
    );

    DIR *dir = opendir(archived_path);
    if (dir == NULL) {
        _log(cwinfo, L_ERROR, "Opening history directory failed.");
        return;
    }

    struct dirent *entry;

    while((entry = readdir(dir)) != NULL) {
        char *entry_name = entry->d_name;
        if(strcmp(entry_name, ".") == 0) continue;
        if(strcmp(entry_name, "..") == 0) continue;

        unsigned long timestamp = strtoul(entry_name, NULL, 10);
        if((unsigned long)current_interval - (timestamp + ARCHIVE_INTERVAL) >= DELETION_INTERVAL) {
            size_t full_entry_path_size = BUFFER_ONE_KB;
            char full_entry_path[full_entry_path_size];

            snprintf(
                full_entry_path, full_entry_path_size,
                "%s/%s",
                archived_path,
                entry_name
            );

            int _remove = remove(full_entry_path); // delete the file that was archived
            if(_remove == -1) {
                _log(cwinfo, L_ERROR, "Removing archive failed.");
            }
            continue;
        }
    }

    closedir(dir);
}

void archiveHistory(CWInfo cwinfo) {
    _archiveHistory(cwinfo, "metric");
    _archiveHistory(cwinfo, "system");
}

void deleteHistoryArchives(CWInfo cwinfo) {
    _deleteHistoryArchives(cwinfo, "metric");
    _deleteHistoryArchives(cwinfo, "system");
}
