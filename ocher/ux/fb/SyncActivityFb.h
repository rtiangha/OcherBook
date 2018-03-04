/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_SYNCACTIVITY_H
#define OCHER_UX_SYNCACTIVITY_H

#include "ux/fb/ActivityFb.h"


class SyncActivityWork;

class SyncActivityFb : public ActivityFb {
public:
    SyncActivityFb(UxControllerFb* c);

    void draw();

protected:
    void onAttached();
    void onDetached();

    FrameBuffer* m_fb;
    SyncActivityWork* m_work;
    Spinner m_spinner;
};

#endif
