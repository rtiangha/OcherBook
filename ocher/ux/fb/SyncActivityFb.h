/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_SYNCACTIVITY_H
#define OCHER_UX_SYNCACTIVITY_H

#include "ux/fb/ActivityFb.h"


class Filesystem;
class SyncActivityWork;

class SyncActivityFb : public ActivityFb {
public:
    SyncActivityFb(UxControllerFb* c, Filesystem& fileystem);

protected:
    void onAttached() override;
    void onDetached() override;
    void draw() override;

    Filesystem& m_filesystem;
    FrameBuffer* m_fb;
    SyncActivityWork* m_work = nullptr;
    Spinner* m_spinner = nullptr;
};

#endif
