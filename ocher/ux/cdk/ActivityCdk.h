/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_CDK_ACTIVITY_H
#define OCHER_UX_CDK_ACTIVITY_H

#include "ux/Activity.h"

class UxControllerCdk;

/** Specialization of Activity for ncurses
 */
class ActivityCdk : public Activity {
public:
    ActivityCdk(UxControllerCdk* uxController) :
        m_uxController(uxController)
    {
    }

    virtual ~ActivityCdk() = default;

protected:
    virtual void onAttached() = 0;
    virtual void onDetached() = 0;

    UxControllerCdk* m_uxController;
};

#endif
