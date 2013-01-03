#include "ocher/ux/Factory.h"
#include "ocher/ux/fb/SystemBar.h"
#include "ocher/settings/Settings.h"


SystemBar::SystemBar(Battery& battery) :
    Window(0, 0, g_fb->width(), 30 /* TODO */),
    m_sep(false),
    m_batteryIcon(settings.smallSpace, 0, battery)
{
    m_borderWidth = 0;
    addChild(&m_batteryIcon);

    // TODO title label, centered
}


void SystemBar::drawContent(Rect* r)
{
    g_fb->setFg(0xff, 0xff, 0xff);
    g_fb->fillRect(&m_rect);
    if (m_sep) {
        g_fb->setFg(0, 0, 0);
        g_fb->hline(r->x, r->y+r->h-2, r->x+r->w-1);
        g_fb->hline(r->x, r->y+r->h-1, r->x+r->w-1);
    }
    if (m_title.length()) {
        FontEngine fe;
        fe.setSize(10);
        fe.setItalic(1);
        fe.setBold(0);
        fe.apply();
        Pos pos;
        pos.x = 0; pos.y = fe.m_cur.ascender + 2;
        fe.renderString(m_title.c_str(), m_title.length(), &pos, &g_fb->bbox, FE_XCENTER);
    }
}
