#ifndef OCHER_KOBO_EVENT_H
#define OCHER_KOBO_EVENT_H

#include <linux/input.h>

#include "ocher/ux/Event.h"


class KoboEvents : public EventLoop
{
public:
    KoboEvents();
    ~KoboEvents();
    int wait(struct OcherEvent* evt);
    void flush();

protected:
    int m_buttonFd;
    int m_touchFd;
    int m_pipe[2];

    struct input_event kevt[64];
    struct OcherEvent m_evt;

    unsigned int kevtHead;  ///< offset of first event in the FIFO
    unsigned int kevtTail;  ///< offset of first empty slot (== head when empty)
};

#endif

