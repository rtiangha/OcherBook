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

    EventDisposition evtKey(const struct OcherKeyEvent*) override;

protected:
    void onAttached() override;
    void onDetached() override;
    void drawContent(const Rect* rect) override;

    Filesystem& m_filesystem;
    SyncActivityWork* m_work = nullptr;
    Spinner* m_spinner = nullptr;
};

#endif
