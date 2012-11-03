#ifndef OCHER_UX_ACTIVITY_H
#define OCHER_UX_ACTIVITY_H

#include "ocher/ux/fb/Widgets.h"


enum Activity {
    ACTIVITY_SYNC,
    ACTIVITY_HOME,
    ACTIVITY_READ,
    ACTIVITY_LIBRARY,
    ACTIVITY_SETTINGS,
    ACTIVITY_QUIT,
};


class OcherCanvas : public Canvas
{
public:
    OcherCanvas();
    ~OcherCanvas();

    void draw(Pos* pos=0);
};

#endif

