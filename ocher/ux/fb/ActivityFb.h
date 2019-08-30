/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_ACTIVITY_H
#define OCHER_UX_FB_ACTIVITY_H

#include "ux/Activity.h"
#include "ux/fb/UxControllerFb.h"
#include "ux/fb/Widgets.h"

class UxControllerFb;

/** Specialization of Activity for framebuffers
 */
class ActivityFb : public Activity, public Window {
public:
    ActivityFb(UxControllerFb* uxController) :
        m_uxController(uxController)
    {
        m_flags |= WIDGET_BORDERLESS;
    }

    virtual ~ActivityFb() = default;

protected:
    EventDisposition evtKey(const struct OcherKeyEvent* evt) override
    {
        if (evt->subtype == OEVT_KEY_DOWN) {
            if (evt->key == OEVTK_HOME) {
                m_uxController->setNextActivity(Activity::Type::Home);
                return EventDisposition::Handled;
            }
        }
        return EventDisposition::Pass;
    }

    UxControllerFb* m_uxController;
};

#endif
