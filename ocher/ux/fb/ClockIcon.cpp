/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/ClockIcon.h"

#include "util/Logger.h"

#include <chrono>
#include <ctime>

#define LOG_NAME "ocher.widgets.clock"

namespace {

struct tm localTime()
{
    const auto now = std::chrono::system_clock::now();
    const auto tt = std::chrono::system_clock::to_time_t(now);
    struct tm currentLocalTime;
    localtime_r(&tt, &currentLocalTime);
    return currentLocalTime;
}

float secondsUntilMinute(const struct tm& now)
{
    float sec = 60.0 - now.tm_sec;
    // beware leap seconds
    if (sec <= 0.0)
        sec = 1.0;
    return sec;
}

}  // end ns

ClockIcon::ClockIcon(int x, int y)
{
    setPos({x, y});

    ev_timer_init(&m_timer, timeoutCb, 0, 1.0);
    m_timer.data = this;

    update();
}

void ClockIcon::resetTimeout(float seconds)
{
    Log::debug(LOG_NAME, "update in %3.1fs", seconds);
    m_timer.repeat = seconds;
    ev_timer_again(m_screen->loop.evLoop, &m_timer);
}

void ClockIcon::update()
{
    char buf[64];
    const auto now = localTime();
    std::strftime(buf, sizeof(buf), "%l:%M", &now);
    Log::info(LOG_NAME, "time is now %s", buf);
    if (!(m_flags & WIDGET_HIDDEN)) {
        erase();
    }
    setLabel(buf);

    resetTimeout(secondsUntilMinute(now));
}

void ClockIcon::timeoutCb(EV_P_ ev_timer* timer, int)
{
    static_cast<ClockIcon*>(timer->data)->update();
}

void ClockIcon::onAttached()
{
    const auto now = localTime();
    resetTimeout(secondsUntilMinute(now));
}

void ClockIcon::onDetached()
{
    ev_timer_stop(m_screen->loop.evLoop, &m_timer);
}
