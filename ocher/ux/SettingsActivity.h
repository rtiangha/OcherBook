#ifndef OCHER_UX_SETTINGSACTIVITY_H
#define OCHER_UX_SETTINGSACTIVITY_H

#include "ocher/ux/Activity.h"


class SettingsActivity
{
public:
    SettingsActivity(Controller* c);

    Activity run();

    Controller* m_controller;
};

#endif

