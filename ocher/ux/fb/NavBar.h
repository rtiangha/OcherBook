/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_NAVBAR_H
#define OCHER_UX_FB_NAVBAR_H

#include "ocher/ux/fb/Widgets.h"


class NavBar : public Window {
public:
    NavBar(FrameBuffer *fb);

protected:
    FrameBuffer *m_fb;
    void drawContent(Rect *);
};

#endif
