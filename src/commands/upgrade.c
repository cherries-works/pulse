#include <inttypes.h>
#include <string.h>
#include <curl/curl.h>
#include <unistd.h>

#include "utils.h"
#include "args.h"

typedef struct {
    char *buffer;
    size_t used;
} HttpBuf;

static size_t writeCB(char *data, size_t size, size_t nmemb, void *userp) {
    HttpBuf *out = userp;
    size_t total = size * nmemb;

    if (total > sizeof(out->buffer) - 1 - out->used) return 0;  // full → abort
    memcpy(out->buffer + out->used, data, total);
    out->used += total;
    out->buffer[out->used] = '\0';
    return total;
}

// v0.6.3 (as example)
// -> 0 * 100 + 6 * 10 + 3 * 1 = 63 (unique hash for a version)
uint16_t parseVersion(const char *data) {
    unsigned long v = 0;
    const char *p = data;
    p++; // skip the first v

    while(*p >= '0' && *p <= '9') {
        v = v * 10 + (unsigned long)(*p - '0');
        p++;
        if (*p == '.') p++;
    }

    return (uint16_t)v;
}

uint16_t fetchLatest() {
    CURL *curl = curl_easy_init();
    if(!curl) return 0;

    char buffer[BUFFER_ONE_KB];
    HttpBuf out = { 
        .used = 0,
        .buffer = buffer,
    };
    out.buffer[0] = '\0';
    char url[BUFFER_ONE_KB];
    snprintf(
        url,
        sizeof(url),
        "%s%s",
        CHERRIES_PULSE_LATEST_URL_HOST,
        CHERRIES_PULSE_LATEST_URL_PATH
    );

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCB);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "pulse-upgrade");

    // everything above 400 is failure
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);  

    CURLcode rc = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    uint16_t v = 0;
    if(rc == CURLE_OK) v = parseVersion(out.buffer);

    return v;
}

void upgrade(CWArgs cwargs) {
    uint16_t v = fetchLatest();
    uint16_t current_v = parseVersion(PULSE_VERSION);
    if(v == current_v) {
        printf("Already on the latest version.\n");
        return;
    }
}