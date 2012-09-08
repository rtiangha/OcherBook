#include <stdio.h>

#include "ocher/device/Filesystem.h"
#include "ocher/ux/Renderer.h"
#include "ocher/ux/fb/BrowseFb.h"
#include "ocher/settings/Options.h"


BrowseFb::BrowseFb()
{
}

bool BrowseFb::init()
{
    return true;
}

Meta* BrowseFb::browse(clc::List& meta)
{
    // TODO  temp hack, just default to the first (if any)
    return (Meta*)meta.get(0);
}

void BrowseFb::read(Renderer* renderer, Meta* meta)
{
    for (int pageNum = 0; ; ) {
        int atEnd = renderer->render(&meta->m_pagination, pageNum, true);

        char key = getchar();
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

