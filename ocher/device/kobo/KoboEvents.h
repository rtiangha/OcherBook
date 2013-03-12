#ifndef OCHER_KOBO_EVENT_H
#define OCHER_KOBO_EVENT_H

#include <linux/input.h>

#include "ocher/ux/Event.h"


class KoboEvents
{
public:
    KoboEvents();
    ~KoboEvents();

    int wait(struct OcherEvent* evt);
    void flush();

protected:
    int m_buttonFd;
    struct ev_io m_buttonWatcher;
    int m_touchFd;
    struct ev_io m_touchWatcher;

    struct input_event kevt[64];
    struct OcherEvent m_evt;

    unsigned int kevtHead;  ///< offset of first event in the FIFO
    unsigned int kevtTail;  ///< offset of first empty slot (== head when empty)
};

#endif

