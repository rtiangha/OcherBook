/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fd/LibraryActivityFd.h"

#include "Container.h"
#include "device/Filesystem.h"
#include "settings/Options.h"
#include "shelf/Meta.h"
#include "ux/fd/UxControllerFd.h"
#include "util/Logger.h"

#include <cstdio>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define LOG_NAME "ocher.ux.Library"

// TODO:  handle !isatty(stdin)

static char getKey()
{
    int key;
    struct termios oldTermios, newTermios;

    tcgetattr(0, &oldTermios);
    newTermios = oldTermios;
    newTermios.c_lflag &= ~(ICANON | ECHO);
    newTermios.c_cc[VTIME] = 0;
    newTermios.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &newTermios);

    key = getc(stdin);

    tcsetattr(0, TCSANOW, &oldTermios);

    return (char)key;
}

LibraryActivityFd::LibraryActivityFd(UxControllerFd* c) :
    ActivityFd(c),
    m_settings(g_container.settings),
    m_library(nullptr),
    m_pageNum(0)
{
}

void LibraryActivityFd::draw()
{
    printf("\n");
    for (unsigned int i = 0; i < m_library->size(); ++i) {
        Meta* m = (*m_library)[i];
        // TODO:  title/author
        // TODO:  pad to width
        printf("%3u: %s\n", i + 1, m->title.c_str());
        printf("     %s\n", m->author.c_str());
        printf("     %4s  %d of %d\n", Meta::fmtToStr(m->format), 0, 100);
    }

    //char key = getKey();
    //return (Meta*)m_library->get(key - '1');
}

void LibraryActivityFd::onAttached()
{
    Log::info(LOG_NAME, "attached");

    m_library = &m_uxController->ctx.library.getList();
    m_pages = (m_library->size() + m_booksPerPage - 1) / m_booksPerPage;
    Log::info(LOG_NAME, "%u books across %u pages", (unsigned)m_library->size(), m_pages);

}

void LibraryActivityFd::onDetached()
{
    Log::info(LOG_NAME, "detached");
}

#if 0
void BrowseFd::read(Meta* meta)
{
    Renderer* renderer = uiFactory->getRenderer();

    for (int pageNum = 0;; ) {
        int atEnd = renderer->render(&meta->m_pagination, pageNum, true);

        char buf[16];
        sprintf(buf, "%u of %u: ", pageNum + 1, meta->m_pagination.numPages() + 1);
        write(m_out, buf, strlen(buf));

        char key = getKey();
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
#endif
