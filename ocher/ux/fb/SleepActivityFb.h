/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_SLEEPACTIVITY_H
#define OCHER_UX_FB_SLEEPACTIVITY_H

#include "ux/fb/ActivityFb.h"

class Device;

class SleepActivityFb : public ActivityFb {
public:
    SleepActivityFb(UxControllerFb* c);

    void onAttached();
    void onDetached();

protected:
    FrameBuffer* m_fb;
    Device* m_device;
    EventLoop* m_loop;
};

#endif
