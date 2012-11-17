#include "clc/support/Logger.h"

#include "ocher/ux/Event.h"


int EventLoop::run(EventHandler* handler)
{
    struct OcherEvent evt;
    while (wait(&evt) == 0) {
        if (evt.type == OEVT_NONE) {
            continue;
        }
        if (evt.type == OEVT_APP && evt.subtype == OEVT_APP_CLOSE) {
            return 0;  // TODO: or worse?
        }
        int r = handler->eventReceived(&evt);
        if (r >= 0)
            return r;
    }
    return 0;
}

