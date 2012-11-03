#include "ocher/ux/Factory.h"
#include "ocher/ux/fb/Widgets.h"
#include "ocher/ux/fb/BatteryIcon.h"


#define BBORDER 1
#define BHEIGHT 12  // height of battery bounding box
#define BWIDTH 25   // width  of battery bounding box

BatteryIcon::BatteryIcon(int x, int y, Battery& battery) :
    Icon(x, y, BWIDTH+BBORDER*2, BHEIGHT+BBORDER*2),
    m_battery(battery)
{
}

void BatteryIcon::draw(Pos* pos)
{
    Rect rect(m_rect);
    rect.offsetBy(pos);

    // TODO: would be faster to unpack an RLE bitmap
    g_fb->setFg(0xff, 0xff, 0xff);
    g_fb->fillRect(&rect);
    g_fb->setFg(0, 0, 0);
    g_fb->hline(rect.x+BBORDER, rect.y+BBORDER, rect.x+BBORDER+BWIDTH-3-1);
    g_fb->hline(rect.x+BBORDER, rect.y+BBORDER+BHEIGHT-1, rect.x+BBORDER+BWIDTH-3-1);
    g_fb->vline(rect.x+BBORDER, rect.y+BBORDER+1, rect.y+BHEIGHT-2-BBORDER); // left
    g_fb->vline(rect.x+BBORDER+BWIDTH-2-1, rect.y+BBORDER+1, rect.y+BBORDER+3); // right top
    g_fb->vline(rect.x+BBORDER+BWIDTH-2-1, rect.y+BHEIGHT-1-BBORDER-1, rect.y+BHEIGHT-1-BBORDER-3); // right bottom
    g_fb->vline(rect.x+BBORDER+BWIDTH-1-1, rect.y+BBORDER+4, rect.y+BHEIGHT-1-BBORDER-4); // right nub

    rect.x += BBORDER+2;
    rect.y += BBORDER+2;
    rect.h = BHEIGHT-4-1;
    rect.w = BWIDTH-4-2-1;
    int percent = m_battery.m_percent;
    if (percent < 0 || percent > 100)
        percent = 100;  // Cap craziness, and treat "unknown" as full (AC?)
    rect.w *= percent;
    rect.w /= 100;

    g_fb->fillRect(&rect);
}



