/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_DEV_POWER_SAVER_H
#define OCHER_DEV_POWER_SAVER_H

#include "ocher/ux/Event.h"

#include "Signal.h"
using namespace Gallant;

class Device;


class PowerSaver {
public:
    PowerSaver();

    void inject(EventLoop *loop);
    void inject(Device *device);

    void setTimeout(unsigned int seconds);

    Signal0<> wantToSleep;

    void sleep();

    void onAttached();
    void onDetached();

protected:
    void resetTimeout();

    static void timeoutCb(EV_P_ ev_timer *w, int revents);
    void timeout();

    void dispatchEvent(const struct OcherEvent *evt);

    EventLoop *m_loop;
    ev_timer m_timer;
    unsigned int m_seconds;

    Device *m_device;
};

#endif
