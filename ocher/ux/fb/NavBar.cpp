/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/NavBar.h"


NavBar::NavBar() :
    m_fb(m_screen->fb)
{
    auto backButton = make_unique<Button>("<<");
    auto forwardButton = make_unique<Button>(">>");

    const int height = backButton->rect().h;
    const int width = backButton->rect().w;

    int top = m_fb->yres() - 1 - height * 2;
    setRect(0, top, m_fb->xres(), height * 2);

    m_backButton = backButton.get();
    backButton->setPos({width, top + height / 2});
    addChild(std::move(backButton));

    m_forwardButton = forwardButton.get();
    forwardButton->setPos({m_backButton->rect().x + m_backButton->rect().w + width / 2, top + height / 2});
    addChild(std::move(forwardButton));
}

void NavBar::drawContent(const Rect* rect)
{
    m_fb->setFg(0xff, 0xff, 0xff);
    m_fb->fillRect(rect);
    m_fb->setFg(0, 0, 0);
    m_fb->hline(rect->x, rect->y, rect->x + rect->w - 1);
}
