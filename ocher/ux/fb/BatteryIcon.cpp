/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/BatteryIcon.h"

#include "device/Battery.h"

BatteryIcon::BatteryIcon(int x, int y, Battery& battery) :
    Widget(x, y, 30, 29),
    m_battery(battery)
{
    ev_timer_init(&m_timer, timeoutCb, 0, 60);
    m_timer.data = this;
}

BatteryIcon::~BatteryIcon()
{
    stopTimer();
}

void BatteryIcon::draw()
{
    FrameBuffer* fb = m_screen->fb;
    Rect rect(m_rect);

    fb->setFg(0xff, 0xff, 0xff);
    fb->fillRect(&rect);
    rect.y += 8;
    rect.h -= 16;
    fb->setFg(0, 0, 0);
    fb->rect(&rect);
    rect.inset(2);
    int percent = m_battery.percent;
    if (percent < 0 || percent > 100)
        percent = 100;  // Cap craziness, and treat "unknown" as full (AC?)
    rect.w *= percent;
    rect.w /= 100;

    fb->fillRect(&rect);
}

void BatteryIcon::startTimer()
{
    ev_timer_start(m_screen->loop.evLoop, &m_timer);
}

void BatteryIcon::stopTimer()
{
    ev_timer_stop(m_screen->loop.evLoop, &m_timer);
}

void BatteryIcon::update()
{
    if (m_battery.readAll() == 0) {
        invalidate();
    }
}

void BatteryIcon::timeoutCb(EV_P_ ev_timer* timer, int)
{
    static_cast<BatteryIcon*>(timer->data)->update();
}

void BatteryIcon::onAttached()
{
    startTimer();
}

void BatteryIcon::onDetached()
{
    stopTimer();
}
