#ifndef APP_H
#define APP_H

#include "http.h"
#include "utils.h"
#include "setup.h"

extern pid_t startWebsite(Args args, struct CWInfo cwinfo);

extern void initRoutes(RouteHandler *rh);

#endif