/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_NAVBAR_H
#define OCHER_UX_FB_NAVBAR_H

#include "ux/fb/Widgets.h"


class NavBar : public Window {
public:
    NavBar();

    Button& backButton() const { return *m_backButton; }
    Button& forwardButton() const { return *m_forwardButton; }

protected:
    void drawContent(const Rect*) override;

    Button* m_backButton;
    Button* m_forwardButton;
};

#endif
