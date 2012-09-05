#include "ocher/ux/Renderer.h"
#include "ocher/ux/ncurses/Browse.h"


BrowseCurses::BrowseCurses()
{
}

bool BrowseCurses::init(CDKSCREEN* screen)
{
    m_screen = screen;
    return true;
}

Meta* BrowseCurses::browse(clc::List& meta)
{
    const unsigned int nItems = meta.size();
    char* items[nItems];

    for (unsigned int i = 0; i < nItems; ++i) {
        Meta* m = (Meta*)meta.get(i);
        items[i] = m->relPath.c_str();
    }

    CDKSCROLL* scroll = newCDKScroll(m_screen, LEFT, TOP, RIGHT, 0, 0, "Browse books...",
            items, nItems, 0, 0, 0, 0);
    int r = activateCDKScroll(scroll, NULL);
    if (r == -1) {
        return (Meta*)0;
    } else {
        return (Meta*)meta.get(r);
    }
}

void BrowseCurses::read(Renderer* renderer, Meta* meta)
{
    for (int pageNum = 0; ; ) {
        if (renderer->render(pageNum, true) < 0)
            return;

#if 0
        clc::Keystroke::Modifiers m;
        clc::Keystroke key = clc::Tui::getKey(&m);
        if (key == 'p' || key == 'b') {
            if (pageNum > 0)
                pageNum--;
        } else if (key == 'q') {
            break;
        } else {
            pageNum++;
        }
#endif
    }
}

