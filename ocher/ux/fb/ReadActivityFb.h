/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_READACTIVITY_H
#define OCHER_UX_FB_READACTIVITY_H

#include "ux/fb/ActivityFb.h"
#include "ux/fb/NavBar.h"
#include "ux/fb/SystemBar.h"

class Meta;
class Renderer;

class ReadActivityFb : public ActivityFb {
public:
    ReadActivityFb(UxControllerFb* c);
    ~ReadActivityFb();

    void turnPages(int n);

protected:
    EventDisposition evtKey(const struct OcherKeyEvent*) override;
    EventDisposition evtMouse(const struct OcherMouseEvent*) override;
    void onAttached() override;
    void onDetached() override;
    void drawContent(const Rect* rect) override;

    void backButtonPressed(Button&);
    void forwardButtonPressed(Button&);

    FrameBuffer* m_fb;
    SystemBar* m_systemBar = nullptr;
    NavBar* m_navBar = nullptr;

    Renderer* m_renderer;
    Meta* meta;
    int m_pageNum;
    int atEnd;
    unsigned int m_pagesSinceRefresh;
};

#endif
