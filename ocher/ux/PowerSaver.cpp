/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/PowerSaver.h"

#include "device/Device.h"
#include "settings/Settings.h"
#include "ux/fb/FontEngine.h"
#include "util/Logger.h"

#include <ev.h>

#define LOG_NAME "ocher.ux.power"


ev_timer timeout_watcher;

void PowerSaver::timeoutCb(EV_P_ ev_timer* timer, int revents)
{
    Log::debug(LOG_NAME, "timeout");

    static_cast<PowerSaver*>(timer->data)->timeout();
}

void PowerSaver::timeout()
{
    wantToSleep();
}

PowerSaver::PowerSaver() :
    m_loop(nullptr),
    m_seconds(15 * 60),
    // TODO settings
    m_device(nullptr)
{
}

void PowerSaver::inject(EventLoop* loop)
{
    m_loop = loop;

    m_loop->emitEvent.Connect(this, &PowerSaver::dispatchEvent);

    resetTimeout();
}

void PowerSaver::inject(Device* device)
{
    m_device = device;
}

void PowerSaver::setTimeout(unsigned int seconds)
{
    m_seconds = seconds;
    resetTimeout();
}

void PowerSaver::resetTimeout()
{
    ev_timer_init(&m_timer, timeoutCb, m_seconds, 0.);
    ev_timer_start(m_loop->evLoop, &m_timer);
    m_timer.data = this;
}

void PowerSaver::dispatchEvent(const struct OcherEvent* evt)
{
    if (evt->type == OEVT_MOUSE) {
        resetTimeout();
    } else if (evt->type == OEVT_KEY) {
        if (evt->key.key == OEVTK_POWER && evt->key.subtype == OEVT_KEY_UP)
            timeout();
        else
            resetTimeout();
    }
}

void PowerSaver::sleep()
{
    m_device->sleep();
}
