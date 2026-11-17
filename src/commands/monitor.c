#include <fcntl.h>

#include "parse.h"
#include "render.h"
#include "utils.h"
#include "http.h"
#include "daemon.h"
#include "log.h"
#include "app.h"
#include "config.h"
#include "setup.h"

void monitor(CWArgs cwargs, CWConfig cwconfig, CWInfo cwinfo) {
    _log(
        cwinfo,
        L_INFO,
        "Running monitor() -> command monitor"
    );
    
    // clean up any leftover from a crash
    sem_unlink(CHERRIES_PULSE_READY_SEM);
    sem_t *ready_sem = sem_open(CHERRIES_PULSE_READY_SEM, O_CREAT | O_EXCL, 0600, 0);
    if (ready_sem == SEM_FAILED) {
        _log(cwinfo, L_ERROR, "Failed to create ready semaphore");
        return;
    }

    startDaemon(cwargs, cwconfig, cwinfo);

    // wait until daemon is ready
    sem_wait(ready_sem);
    sem_post(ready_sem);
    sem_close(ready_sem);
    sem_unlink(CHERRIES_PULSE_READY_SEM);

    if(cwargs.headless && !cwargs.web) return;

    if(cwargs.web) {
        startWebsite(cwargs, cwinfo);
    }

    if(!cwargs.headless) {
        startRender(cwargs, cwinfo);
    }
}
