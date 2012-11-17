#include "ocher/ux/Factory.h"
#include "ocher/ux/fb/SystemBar.h"


SystemBar::SystemBar(Battery& battery) :
    Window(0, 0, g_fb->width(), 26 /* TODO */),
    m_batteryIcon(1, 1, battery)
{
    m_borderWidth = 0;
    addChild(&m_batteryIcon);

    // TODO clock
}


void SystemBar::drawContent(Rect* r)
{
    g_fb->setFg(0xff, 0xff, 0xff);
    g_fb->fillRect(&m_rect);
    g_fb->setFg(0, 0, 0);
    g_fb->hline(r->x, r->y+r->h-2, r->x+r->w-1);
    g_fb->hline(r->x, r->y+r->h-1, r->x+r->w-1);
}
