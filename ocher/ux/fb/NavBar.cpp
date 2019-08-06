/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/NavBar.h"


NavBar::NavBar() :
    m_fb(m_screen->fb)
{
    const int height = 50;
    int top = m_fb->yres() - 1 - height;
    setRect(0, top, m_fb->xres(), height);

    auto button = make_unique<Button>("<<");
    m_backButton = button.get();
    button->setPos({20, top + 5});
    addChild(std::move(button));

    button = make_unique<Button>(">>");
    m_forwardButton = button.get();
    button->setPos({m_backButton->rect().x + m_backButton->rect().w + 20, top + 5});
    addChild(std::move(button));
}

void NavBar::drawContent(const Rect* rect)
{
    m_fb->setFg(0xff, 0xff, 0xff);
    m_fb->fillRect(rect);
    m_fb->setFg(0, 0, 0);
    m_fb->hline(rect->x, rect->y, rect->x + rect->w - 1);
}
