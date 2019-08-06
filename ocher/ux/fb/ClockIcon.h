/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_CLOCKICON_H
#define OCHER_UX_FB_CLOCKICON_H

#include "ux/fb/Widgets.h"


class ClockIcon : public Label {
public:
    ClockIcon(int x, int y);
    ~ClockIcon();

protected:
    void onAttached() override;
    void onDetached() override;

    void resetTimeout(float seconds);
    void clearTimeout();
    void update();

    ev_timer m_timer;
    static void timeoutCb(EV_P_ ev_timer* w, int revents);
};

#endif
