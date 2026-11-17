#ifndef APP_H
#define APP_H

#include "http.h"
#include "args.h"

extern pid_t startWebsite(CWArgs args, CWInfo cwinfo);

extern void initRoutes(RouteHandler *rh);

#endif