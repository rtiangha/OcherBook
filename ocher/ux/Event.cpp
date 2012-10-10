#include "ocher/ux/Event.h"


int EventLoop::run(EventHandler* handler)
{
    struct OcherEvent evt;
    while (wait(&evt) == 0) {
        if (evt.type == OEVT_NONE) {
            ;
        }
        int r = handler->eventReceived(&evt);
        if (r >= 0)
            return r;
    }
    return 0;
}

