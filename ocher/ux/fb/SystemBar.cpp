/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/SystemBar.h"

#include "Container.h"
#include "settings/Settings.h"


SystemBar::SystemBar(Battery* battery) :
    m_sep(false),
    m_fb(m_screen->fb),
    m_batteryIcon(g_container.settings->smallSpace, 0, battery),
    m_clockIcon(m_fb->width() - 50, 0)
{
    setRect(0, 0, m_fb->width(), 30);

    m_borderWidth = 0;
    addChild(m_batteryIcon);
    addChild(m_clockIcon);

    // TODO title label, centered
}


void SystemBar::drawContent(Rect* r)
{
    m_fb->setFg(0xff, 0xff, 0xff);
    m_fb->fillRect(&m_rect);
    if (m_sep) {
        m_fb->setFg(0, 0, 0);
        m_fb->hline(r->x, r->y + r->h - 1, r->x + r->w - 1);
    }
    if (m_title.length()) {
        /* TODO  need factories...
           FontEngine fe;
           fe.setSize(10);
           fe.setItalic(0);
           fe.setBold(0);
           fe.apply();
           Pos pos;
           pos.x = 0; pos.y = fe.m_cur.ascender + 2;
           fe.renderString(m_title.c_str(), m_title.length(), &pos, &m_fb->bbox, FE_XCENTER);
         */
    }
}
