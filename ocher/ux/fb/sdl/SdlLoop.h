#ifndef OCHER_SDL_LOOP_H
#define OCHER_SDL_LOOP_H

#include "clc/os/Monitor.h"
#include "clc/os/Thread.h"
#include "clc/data/List.h"
#include "ocher/ux/Event.h"


class SdlLoop : public clc::Thread
{
public:
    SdlLoop();
    ~SdlLoop();

    void start(EventLoop* loop, clc::Monitor* startupMonitor);
    void stop();

    SDL_Surface* getScreen() { return m_screen; }

protected:
    SDL_Surface* init();
    void run();

    clc::List m_q;

    int m_pipe[2];
    EventLoop* m_loop;
    clc::Monitor* m_startupMonitor;
    SDL_Surface* m_screen;
};

#endif

