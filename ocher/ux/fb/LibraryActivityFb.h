/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_LIBRARYACTIVITY_H
#define OCHER_UX_FB_LIBRARYACTIVITY_H

#include "ux/fb/ActivityFb.h"
#include "ux/fb/SystemBar.h"

class Meta;
class Settings;

class LibraryActivityFb : public ActivityFb {
public:
    LibraryActivityFb(UxControllerFb* c);
    ~LibraryActivityFb();

protected:
    EventDisposition evtKey(const struct OcherKeyEvent*) override;
    EventDisposition evtMouse(const struct OcherMouseEvent*) override;
    void onAttached() override;
    void drawContent(const Rect* rect) override;

    void leftIconPressed();
    void rightIconPressed();

    Settings& m_settings;
    SystemBar* m_systemBar = nullptr;
#define BOOKS_PER_PAGE 11
    Rect* m_bookRects;
    int itemHeight;
    unsigned int m_booksPerPage;
    unsigned int m_pages;
    unsigned int m_pageNum;
};

#endif
