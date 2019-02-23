/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_SLEEPACTIVITY_H
#define OCHER_UX_FB_SLEEPACTIVITY_H

#include "ux/fb/ActivityFb.h"

class Device;

enum class PowerLevel {
    Sleep,
    PowerOff,
};

class SleepActivityFb : public ActivityFb {
public:
    SleepActivityFb(UxControllerFb* c, PowerLevel level);

protected:
    void onAttached() override;
    void drawContent(Rect* rect) override;

    static void idleCb(EV_P_ ev_idle* w, int revents);
    void tired();
    void awake();
    void done();

    enum class Disposition {
        Tired,
        Awake,
        Done,
    };
    Disposition m_state = Disposition::Tired;
    PowerLevel m_level;

    FrameBuffer* m_fb;
    EventLoop& m_loop;
    ev_idle m_idle;

    Activity::Type m_previousActivity;
};

#endif
