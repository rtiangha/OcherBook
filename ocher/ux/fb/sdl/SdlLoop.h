#ifndef OCHER_SDL_LOOP_H
#define OCHER_SDL_LOOP_H

#include "ocher/ux/Event.h"


class SdlLoop : public EventLoop
{
public:
    int wait(struct OcherEvent* evt);
};

#endif

