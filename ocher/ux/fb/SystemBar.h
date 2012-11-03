#ifndef OCHER_UX_FB_SYSTEMBAR_H
#define OCHER_UX_FB_SYSTEMBAR_H

#include "ocher/ux/fb/Widgets.h"
#include "ocher/ux/fb/BatteryIcon.h"


class SystemBar : public Window
{
public:
    SystemBar(Battery& battery);

protected:
    void drawContent(Rect*);

    BatteryIcon m_batteryIcon;
};

#endif

