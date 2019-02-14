/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/PowerSaver.h"

#include "settings/Settings.h"
#include "ux/fb/FontEngine.h"
#include "util/Logger.h"

#include <ev.h>

#define LOG_NAME "ocher.ux.power"


ev_timer timeout_watcher;

void PowerSaver::timeoutCb(EV_P_ ev_timer* timer, int revents)
{
    static_cast<PowerSaver*>(timer->data)->timeout();
}

void PowerSaver::timeout()
{
    Log::debug(LOG_NAME, "timeout");
    if (m_powerKeyDown && now() - m_powerKeyDownTime > m_holdPoweroffSeconds) {
        Log::debug(LOG_NAME, "want poweroff");
        m_poweringOff = true;
        wantToPowerOff();
    } else if (now() - m_activityTime > m_sleepSeconds) {
        Log::debug(LOG_NAME, "want sleep");
        wantToSleep();
    }
}

PowerSaver::PowerSaver(EventLoop& loop) :
    m_loop(loop)
{
    m_loop.emitEvent.Connect(this, &PowerSaver::dispatchEvent);

    ev_timer_init(&m_timer, timeoutCb, 0., m_sleepSeconds);
    m_timer.data = this;

    activity();
}

void PowerSaver::setSleepTimeout(int seconds)
{
    m_sleepSeconds = seconds;
    resetTimeout(m_sleepSeconds);
}

ev_tstamp PowerSaver::now()
{
    return ev_now(m_loop.evLoop);
}

void PowerSaver::activity()
{
    m_activityTime = now();
    resetTimeout(m_sleepSeconds);
}

void PowerSaver::resetTimeout(int seconds)
{
    m_timer.repeat = seconds;
    ev_timer_again(m_loop.evLoop, &m_timer);
}

void PowerSaver::dispatchEvent(const struct OcherEvent* evt)
{
    if (evt->type == OEVT_MOUSE) {
        activity();
    } else if (evt->type == OEVT_KEY) {
        if (evt->key.key == OEVTK_POWER) {
            if (evt->key.subtype == OEVT_KEY_DOWN) {
                Log::debug(LOG_NAME, "power key down");
                m_powerKeyDown = true;
                m_powerKeyDownTime = now();
                resetTimeout(m_holdPoweroffSeconds);
            } else if (evt->key.subtype == OEVT_KEY_UP) {
                Log::debug(LOG_NAME, "power key up");
                m_powerKeyDown = false;
                if (!m_poweringOff) {
                    activity();
                    wantToSleep();
                }
            }
        } else {
            activity();
        }
    }
}
