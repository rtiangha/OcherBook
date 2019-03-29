/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/NavBar.h"


NavBar::NavBar() :
    m_fb(m_screen->fb)
{
    setRect(0, m_fb->yres() - 50, m_fb->xres(), 50);
}

void NavBar::drawContent(const Rect* rect)
{
    m_fb->setFg(0xff, 0xff, 0xff);
    m_fb->fillRect(rect);
    m_fb->setFg(0, 0, 0);
    m_fb->hline(rect->x, rect->y, rect->x + rect->w - 1);
}
