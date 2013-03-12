#ifndef OCHER_UX_FB_SYSTEMBAR_H
#define OCHER_UX_FB_SYSTEMBAR_H

#include "clc/data/Buffer.h"
#include "ocher/ux/fb/Widgets.h"
#include "ocher/ux/fb/BatteryIcon.h"
#include "ocher/ux/fb/ClockIcon.h"


class SystemBar : public Window
{
public:
    SystemBar(Battery& battery);

    bool m_sep;
    clc::Buffer m_title;

protected:
    void drawContent(Rect*);

    BatteryIcon m_batteryIcon;
    ClockIcon m_clockIcon;
};

#endif
