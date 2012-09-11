#include <stdio.h>

#include "ocher/device/Filesystem.h"
#include "ocher/settings/Options.h"
#include "ocher/shelf/Meta.h"
#include "ocher/ux/fb/FactoryFb.h"
#include "ocher/ux/fb/FrameBuffer.h"


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

void BrowseFb::read(UiFactory* _factory, Meta* meta)
{
    UiFactoryFb* factory = (UiFactoryFb*)_factory;
    Renderer* renderer = factory->getRenderer();
    EventLoop* loop = factory->getLoop();
    FrameBuffer* fb = factory->getFrameBuffer();

    for (int pageNum = 0; ; ) {
        int atEnd = renderer->render(&meta->m_pagination, pageNum, true);

        struct OcherEvent evt;
        int oldPageNum = pageNum;
        while (pageNum == oldPageNum) {
            if (loop->wait(&evt) == 0) {
                switch (evt.type) {
                    case OEVT_KEY:
                        if (evt.subtype == OEVT_KEY_DOWN) {
                            if (evt.key.key == OEVTK_HOME)
                                return;
                        }
                        break;
                    case OEVT_MOUSE:
                        if (evt.subtype == OEVT_MOUSE1_CLICKED) {
                            if (evt.mouse.x < fb->width()/2) {
                                if (pageNum > 0)
                                    pageNum--;
                            } else {
                                pageNum++;
                            }
                            break;
                        }
                    case OEVT_APP:
                        if (evt.subtype == OEVT_APP_CLOSE)
                            exit(0);
                }
            }
        }
    }
}

