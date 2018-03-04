/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/cdk/LibraryActivityCdk.h"

#include "shelf/Meta.h"
#include "util/Logger.h"
#include "ux/cdk/RendererCdk.h"
#include "ux/cdk/UxControllerCdk.h"

#include <cdk.h>

#define LOG_NAME "ocher.ux.Library"


LibraryActivityCdk::LibraryActivityCdk(UxControllerCdk* uxController) :
    ActivityCdk(uxController)
{
}

void LibraryActivityCdk::onAttached()
{
    Log::info(LOG_NAME, "attached");

}

void LibraryActivityCdk::onDetached()
{
    Log::info(LOG_NAME, "detached");
}

Meta* LibraryActivityCdk::browse(std::vector<Meta*>& meta)
{
    const unsigned int nItems = meta.size();
    const char* items[nItems];

    for (unsigned int i = 0; i < nItems; ++i) {
        Meta* m = meta[i];
        items[i] = m->relPath.c_str();
    }

    // TODO:  Is this really char**?
    CDKSCROLL* scroll = newCDKScroll(m_uxController->m_screen, LEFT, TOP, RIGHT, 0, 0,
            "Select a book...", (char**)items, nItems, 0, 0, 1, 1);
    int r = activateCDKScroll(scroll, nullptr);
    destroyCDKScroll(scroll);
    if (r == -1) {
        return nullptr;
    } else {
        return meta[r];
    }
}

void LibraryActivityCdk::read(Meta* meta)
{
    Renderer* renderer = m_uxController->getRenderer();

    for (int pageNum = 0;; ) {
        int atEnd = renderer->render(&meta->m_pagination, pageNum, true);

        int key = getch();
        if (key == 'p' || key == 'b') {
            if (pageNum > 0)
                pageNum--;
        } else if (key == 'q' || atEnd) {
            break;
        } else {
            pageNum++;
        }
    }
}
