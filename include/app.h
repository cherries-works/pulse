#ifndef APP_H
#define APP_H

#include "http.h"
#include "utils.h"

extern void initRoutes(RouteHandler *rh);
extern void startWebsite(PulseArgs args);

#endif