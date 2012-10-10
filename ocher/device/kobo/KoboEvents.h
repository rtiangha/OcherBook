#ifndef OCHER_KOBO_EVENT_H
#define OCHER_KOBO_EVENT_H

#include "ocher/ux/Event.h"


class KoboEvents : public EventLoop
{
public:
    KoboEvents();
    ~KoboEvents();
    int wait(struct OcherEvent* evt);

protected:
    int m_buttonFd;
    int m_touchFd;
    int m_pipe[2];
};

#endif

