#ifndef OCHER_UX_FB_CLOCKICON_H
#define OCHER_UX_FB_CLOCKICON_H

#include "ocher/ux/fb/Widgets.h"


class ClockIcon : public Widget
{
public:
    ClockIcon(int x, int y);

    Rect draw(Pos* pos);

    void onActivate(bool active);

protected:
    bool m_active;
};

#endif
