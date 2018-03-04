/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FD_LIBRARYACTIVITY_H
#define OCHER_UX_FD_LIBRARYACTIVITY_H

#include "ux/fd/ActivityFd.h"

#include <vector>

class Meta;
class Settings;

class LibraryActivityFd : public ActivityFd {
public:
    LibraryActivityFd(UxControllerFd* c);

    void draw();

    int evtKey(struct OcherKeyEvent*);
    int evtMouse(struct OcherMouseEvent*);

protected:
    void onAttached();
    void onDetached();

    Settings* m_settings;
    const std::vector<Meta*>* m_library;
#define BOOKS_PER_PAGE 11
    int itemHeight;
    unsigned int m_booksPerPage;
    unsigned int m_pages;
    unsigned int m_pageNum;
};

#endif
