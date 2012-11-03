#include "ocher/ux/Factory.h"
#include "ocher/ux/fb/SystemBar.h"


SystemBar::SystemBar(Battery& battery) :
    Window(0, 0, g_fb->width(), 20),
    m_batteryIcon(0, 0, battery)
{
    m_borderWidth = 0;
    addChild(&m_batteryIcon);
}


void SystemBar::drawContent(Rect* r)
{
    g_fb->setFg(0, 0, 0);
    g_fb->hline(r->x, r->y+r->h-2, r->x+r->w-1);
    g_fb->hline(r->x, r->y+r->h-1, r->x+r->w-1);
}
