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
    BatteryIcon(int x, int y, Battery* battery);

    void draw();

    void onUpdate();

    Battery *m_battery;
};

#endif
