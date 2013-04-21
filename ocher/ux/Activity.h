#ifndef OCHER_UX_ACTIVITY_H
#define OCHER_UX_ACTIVITY_H

#include "ocher/device/Battery.h"
#include "ocher/ux/fb/BatteryIcon.h"
#include "ocher/ux/fb/NavBar.h"
#include "ocher/ux/fb/SystemBar.h"
#include "ocher/ux/fb/Widgets.h"


class Controller;

enum Activity {
    ACTIVITY_SYNC,
    ACTIVITY_HOME,
    ACTIVITY_READ,
    ACTIVITY_LIBRARY,
    ACTIVITY_SETTINGS,

    ACTIVITY_PREVIOUS,
    ACTIVITY_QUIT,
};


/**
 * Optional shared UI components, available to all Activities.
 */
class UiBits
{
public:
    UiBits() : m_systemBar(m_battery) {}

    Battery m_battery;
    SystemBar m_systemBar;
    NavBar m_navBar;
};

#endif
