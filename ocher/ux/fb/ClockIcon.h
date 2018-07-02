/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_CLOCKICON_H
#define OCHER_UX_FB_CLOCKICON_H

#include "ux/fb/Widgets.h"


class ClockIcon : public Widget {
public:
    ClockIcon(int x, int y);

protected:
    void onAttached() override;
    void onDetached() override;
    void draw() override;

    bool m_active;
};

#endif
