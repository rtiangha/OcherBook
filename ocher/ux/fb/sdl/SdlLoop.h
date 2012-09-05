#ifndef OCHER_SDL_LOOP_H
#define OCHER_SDL_LOOP_H

#include "clc/os/Thread.h"

class Controller;

class SdlLoop : clc::Thread
{
public:
    SdlLoop(Controller *c);

protected:
    void run();

};

#endif

