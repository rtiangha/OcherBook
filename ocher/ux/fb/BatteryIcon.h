#ifndef OCHER_UX_FB_BATTERYICON_H
#define OCHER_UX_FB_BATTERYICON_H

#include "ocher/ux/fb/Widgets.h"
#include "ocher/device/Battery.h"


class BatteryIcon : public Icon
{
public:
    BatteryIcon(int x, int y, Battery& battery);

    void draw(Pos* pos);

    Battery& m_battery;
};

#endif

