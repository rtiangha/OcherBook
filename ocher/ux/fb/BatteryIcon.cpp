#include "ocher/ux/Factory.h"
#include "ocher/ux/fb/Widgets.h"
#include "ocher/ux/fb/BatteryIcon.h"


#define BBORDER 1
#define BHEIGHT 18  // height of battery bounding box
#define BWIDTH 25   // width  of battery bounding box

BatteryIcon::BatteryIcon(int x, int y, Battery& battery) :
    Widget(x, y, 30, 30), //BWIDTH+BBORDER*2, BHEIGHT+BBORDER*2),
    m_battery(battery)
{
}

Rect BatteryIcon::draw(Pos* pos)
{
    Rect rect(m_rect);
    rect.offsetBy(pos);

    if (! (m_flags & WIDGET_DIRTY)) {
        // not bothering with children...
        Rect drawn;
        drawn.setInvalid();
        return drawn;
    }
    m_flags &= ~WIDGET_DIRTY;

    g_fb->setFg(0xff, 0xff, 0xff);
    g_fb->fillRect(&rect);
    rect.y += 8;
    rect.h -= 16;
    g_fb->setFg(0, 0, 0);
    g_fb->rect(&rect);
    rect.inset(2);
    int percent = m_battery.m_percent;
    if (percent < 0 || percent > 100)
        percent = 100;  // Cap craziness, and treat "unknown" as full (AC?)
    rect.w *= percent;
    rect.w /= 100;

    g_fb->fillRect(&rect);

    Rect drawn = rect;
    return drawn;
}



