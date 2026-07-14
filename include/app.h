#ifndef APP_H
#define APP_H

#include "http.h"
#include "utils.h"

extern pid_t startWebsite(Args args);

extern void initRoutes(RouteHandler *rh);

#endif