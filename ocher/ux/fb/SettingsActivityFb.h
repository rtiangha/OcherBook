/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_SETTINGSACTIVITY_H
#define OCHER_UX_FB_SETTINGSACTIVITY_H

#include "ocher/ux/fb/ActivityFb.h"


class SettingsActivityFb : public ActivityFb {
public:
    SettingsActivityFb(UxControllerFb *c);

protected:
    void onAttached();
    void onDetached();
};

#endif
