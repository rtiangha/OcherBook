#ifndef OCHER_UX_READACTIVITY_H
#define OCHER_UX_READACTIVITY_H

#include "clc/data/Buffer.h"
#include "ocher/ux/Activity.h"
#include "ocher/ux/fb/Widgets.h"


class Renderer;

class ReadActivity : public Canvas
{
public:
    ReadActivity(UiBits& ui);

    Activity run();

protected:
    int evtKey(struct OcherEvent*);
    int evtMouse(struct OcherEvent*);

    Rect draw(Pos* pos);

    UiBits& m_ui;

    Activity m_next;

    clc::Buffer m_layout;
    Renderer* renderer;
    Meta* meta;
    unsigned int m_pageNum;
    int atEnd;
    unsigned int m_pagesSinceRefresh;
};

#endif

