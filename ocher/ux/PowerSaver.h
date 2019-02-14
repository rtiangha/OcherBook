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

    void setSleepTimeout(int seconds);

    Signal0<> wantToSleep;
    Signal0<> wantToPowerOff;

protected:
    ev_tstamp now();
    void activity();
    void resetTimeout(int seconds);

    static void timeoutCb(EV_P_ ev_timer* w, int revents);
    void timeout();

    void dispatchEvent(const struct OcherEvent* evt);

    EventLoop& m_loop;
    ev_timer m_timer;

    // sleep
    int m_sleepSeconds = 15 * 60;
    ev_tstamp m_activityTime = 0;

    // power key
    ev_tstamp m_powerKeyDownTime = 0;
    bool m_powerKeyDown = false;
    bool m_poweringOff = false;
    int m_holdPoweroffSeconds = 4;
};

#endif
