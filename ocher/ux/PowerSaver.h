/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_POWER_SAVER_H
#define OCHER_UX_POWER_SAVER_H

#include "ux/Event.h"

#include "Signal.h"
using namespace Gallant;


class PowerSaver {
public:
    PowerSaver(EventLoop& loop);

    void setTimeout(unsigned int seconds);

    Signal0<> wantToSleep;

protected:
    void resetTimeout();

    static void timeoutCb(EV_P_ ev_timer* w, int revents);
    void timeout();

    void dispatchEvent(const struct OcherEvent* evt);

    EventLoop& m_loop;
    ev_timer m_timer;
    unsigned int m_seconds;
};

#endif
