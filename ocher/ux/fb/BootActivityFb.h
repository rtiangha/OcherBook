/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_BOOTACTIVITY_H
#define OCHER_UX_FB_BOOTACTIVITY_H

#include "ux/fb/ActivityFb.h"


class BootActivityFb : public ActivityFb {
public:
    BootActivityFb(UxControllerFb* uxController);

    void draw();

    EventDisposition evtMouse(const struct OcherMouseEvent*);

    void onAttached();
    void onDetached();

protected:
    FrameBuffer* m_fb;

    void highlight(int i);
    Rect apps[2];
};

#endif
