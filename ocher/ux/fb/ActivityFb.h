/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_ACTIVITY_H
#define OCHER_UX_FB_ACTIVITY_H

#include "ocher/ux/Activity.h"
#include "ocher/ux/fb/Widgets.h"

class UxControllerFb;

/** Specialization of Activity for framebuffers
 */
class ActivityFb : public Activity, public Window {
public:
    ActivityFb(UxControllerFb *uxController) :
        m_uxController(uxController)
    {
    }

    virtual ~ActivityFb()
    {
    }

    virtual void onAttached() = 0;
    virtual void onDetached() = 0;

    UxControllerFb *m_uxController;
};

#endif
