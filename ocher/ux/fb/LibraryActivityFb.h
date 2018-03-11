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

    void draw();

    int evtKey(const struct OcherKeyEvent*);
    int evtMouse(const struct OcherMouseEvent*);

protected:
    void onAttached();
    void onDetached();

    SystemBar* m_systemBar;
    Settings* m_settings;
#define BOOKS_PER_PAGE 11
    Rect* m_bookRects;
    int itemHeight;
    unsigned int m_booksPerPage;
    unsigned int m_pages;
    unsigned int m_pageNum;
};

#endif
