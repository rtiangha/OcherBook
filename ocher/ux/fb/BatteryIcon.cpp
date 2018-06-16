/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/BatteryIcon.h"

#include "device/Battery.h"


#define BBORDER 1
#define BHEIGHT 18  // height of battery bounding box
#define BWIDTH 25   // width  of battery bounding box

BatteryIcon::BatteryIcon(int x, int y, Battery* battery) :
    Widget(x, y, 30, 29),
    // BWIDTH+BBORDER*2, BHEIGHT+BBORDER*2),
    m_battery(battery)
{
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
    int percent = m_battery->m_percent;
    if (percent < 0 || percent > 100)
        percent = 100;  // Cap craziness, and treat "unknown" as full (AC?)
    rect.w *= percent;
    rect.w /= 100;

    fb->fillRect(&rect);
}

void BatteryIcon::onUpdate()
{
    m_battery->readAll();
}
