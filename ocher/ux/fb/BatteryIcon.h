/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_BATTERYICON_H
#define OCHER_UX_FB_BATTERYICON_H

#include "ux/fb/Widgets.h"

class Battery;

class BatteryIcon : public Widget {
public:
    BatteryIcon(int x, int y, Battery& battery);
    ~BatteryIcon();

protected:
    void onAttached() override;
    void onDetached() override;

    void startTimer();
    void stopTimer();
    void update();

    ev_timer m_timer;
    static void timeoutCb(EV_P_ ev_timer* w, int revents);

    void draw() override;

    Battery& m_battery;
};

#endif
