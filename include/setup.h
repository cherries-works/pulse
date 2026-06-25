#ifndef SETUP_H
#define SETUP_H

#include <signal.h>

extern volatile sig_atomic_t running;
extern int setup();

#endif