/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_KOBO_EVENT_H
#define OCHER_KOBO_EVENT_H

#include "ux/Event.h"

class KoboEvents {
public:
    KoboEvents(EventLoop& loop);
    ~KoboEvents();

protected:
    int m_buttonFd;
    struct ev_io m_buttonWatcher;
    static void buttonCb(struct ev_loop* loop, ev_io* watcher, int revents);
    void pollButton();

    int m_touchFd;
    struct ev_io m_touchWatcher;
    static void touchCb(struct ev_loop* loop, ev_io* watcher, int revents);
    void pollTouch();

    EventLoop& m_loop;
    OcherEvent m_evt;
};

#endif
