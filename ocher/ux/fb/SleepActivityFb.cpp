/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/SleepActivityFb.h"

#include "Container.h"
#include "device/Device.h"
#include "util/Logger.h"
#include "ux/fb/FontEngine.h"
#include "ux/fb/UxControllerFb.h"

#include <unistd.h>

#define LOG_NAME "ocher.ux.sleep"

SleepActivityFb::SleepActivityFb(UxControllerFb* c, PowerLevel level) :
    ActivityFb(c),
    m_level(level),
    m_fb(c->getFrameBuffer()),
    m_loop(g_container->loop)
{
}

void SleepActivityFb::idleCb(EV_P_ ev_idle* w, int revents)
{
    auto self = static_cast<SleepActivityFb*>(w->data);
    if (self->m_state == Disposition::Tired)
        self->tired();
    else if (self->m_state == Disposition::Awake)
        self->awake();
    else
        self->done();
}

void SleepActivityFb::tired()
{
    Log::debug(LOG_NAME, "tired");

    auto& device = g_container->device;
    // TODO fade backlight out
    ::sleep(1);
    if (m_level == PowerLevel::Sleep)
        device->sleep();
    else
        device->poweroff();

    m_state = Disposition::Awake;
}

void SleepActivityFb::awake()
{
    Log::debug(LOG_NAME, "awake");

    m_loop.setEpoch();  // Toss old events
    // TODO restore backlight

    m_state = Disposition::Done;
}

void SleepActivityFb::done()
{
    Log::debug(LOG_NAME, "done");

    ev_idle_stop(m_loop.evLoop, &m_idle);

    g_container->uxController->setNextActivity(m_previousActivity);
}

void SleepActivityFb::onAttached()
{
    m_winflags |= WIDGET_BORDERLESS;
    maximize();

    m_previousActivity = g_container->uxController->previousActivity();

    m_state = Disposition::Tired;
    ev_idle_init(&m_idle, idleCb);
    m_idle.data = this;
    ev_idle_start(m_loop.evLoop, &m_idle);
}

void SleepActivityFb::drawContent(Rect* rect)
{
    FontEngine fe(m_fb);
    fe.setSize(18);
    fe.setItalic(1);
    fe.apply();

    Pos p;
    p.x = 0;
    p.y = rect->h / 2;
    const char* msg = m_level == PowerLevel::Sleep ? "Sleeping" : "Powered off";
    fe.renderString(msg, strlen(msg), &p, rect, FE_XCENTER);

    m_fb->update(rect, true); // Full refresh to clear page remnants
    m_fb->sync();
}
