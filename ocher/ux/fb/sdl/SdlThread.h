/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_SDL_LOOP_H
#define OCHER_SDL_LOOP_H

#include "ocher/ux/Event.h"
#include "ocher/util/Lock.h"
#include "ocher/util/Monitor.h"
#include "ocher/util/Thread.h"

#include "SDL/SDL.h"

#include <list>


/**
 * Thread to initialize SDL graphics and gather SDL events.  Note that SDL must do these two things
 * on the same thread, but the main thread of OcherBook knows nothing of SDL.  Therefore SDL runs on
 * its own thread, and feeds events back to the main thread via libev to be fired there.
 */
class SdlThread : public Thread {
public:
    SdlThread();
    ~SdlThread();

    void setEventLoop(EventLoop *loop);

    void start(Monitor *startupMonitor);
    void stop();

    SDL_Surface *getScreen()
    {
        return m_screen;
    }

protected:
    SDL_Surface *init();
    void run();

    Lock m_evtLock;
    std::list<OcherEvent *> m_evtQ;
    int m_pipe[2];
    struct ev_io m_evtWatcher;
    static void fireEventsCb(struct ev_loop *, ev_io *watcher, int);
    void fireEvents();

    EventLoop *m_loop;
    Monitor *m_startupMonitor;
    SDL_Surface *m_screen;
};

#endif
