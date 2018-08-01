/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "device/kobo/KoboEvents.h"

#include "util/Debug.h"
#include "util/Logger.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

#define LOG_NAME "ocher.dev.kobo"


KoboEvents::KoboEvents(EventLoop& loop) :
    m_loop(loop)
{
    m_buttonFd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
    if (m_buttonFd != -1) {
        ev_io_init(&m_buttonWatcher, buttonCb, m_buttonFd, EV_READ);
        m_buttonWatcher.data = this;
        ev_io_start(m_loop.evLoop, &m_buttonWatcher);
    }

    m_touchFd = open("/dev/input/event1", O_RDONLY | O_NONBLOCK);
    if (m_touchFd != -1) {
        ev_io_init(&m_touchWatcher, touchCb, m_touchFd, EV_READ);
        m_touchWatcher.data = this;
        ev_io_start(m_loop.evLoop, &m_touchWatcher);
    }
}

KoboEvents::~KoboEvents()
{
    if (m_buttonFd != -1) {
        close(m_buttonFd);
    }
    if (m_touchFd != -1) {
        close(m_touchFd);
    }
}

void KoboEvents::buttonCb(struct ev_loop* , ev_io* watcher, int)
{
    static_cast<KoboEvents*>(watcher->data)->pollButton();
}

void KoboEvents::pollButton()
{
    while (true) {
        struct input_event ie;
        int r = read(m_buttonFd, &ie, sizeof(ie));
        if (r == -1) {
            if (errno == EINTR)
                continue;
            ASSERT(errno == EAGAIN || errno == EWOULDBLOCK);
            break;
        } else if (r == sizeof(ie)) {
            bool fire = true;
            OcherEvent evt;
            Log::debug(LOG_NAME, "type %u code %u value %d", ie.type, ie.code, ie.value);
            switch (ie.type) {
            case EV_KEY:
                evt.type = OEVT_KEY;
                if (ie.code == KEY_HOME) {
                    evt.key.subtype = ie.value ? OEVT_KEY_DOWN : OEVT_KEY_UP;
                    evt.key.key = OEVTK_HOME;
                } else if (ie.code == KEY_POWER) {
                    evt.key.subtype = ie.value ? OEVT_KEY_DOWN : OEVT_KEY_UP;
                    evt.key.key = OEVTK_POWER;
                } else {
                    fire = false;
                }
                if (fire) {
                    m_loop.emitEvent(&evt);
                }
                break;
            default:
                ;
            }
        } else {
            break;
        }
    }
}

void KoboEvents::touchCb(struct ev_loop*, ev_io* watcher, int)
{
    static_cast<KoboEvents*>(watcher->data)->pollTouch();
}

void KoboEvents::pollTouch()
{
    // If this is not enough, libev will call again.  Simple is good.
    struct input_event kevt[64];

    int r;

    do {
        r = read(m_touchFd, kevt, sizeof(kevt));
    } while (r == -1 && errno == EINTR);

    unsigned int n = r / sizeof(struct input_event);
    Log::trace(LOG_NAME, "read %u events", n);

    // http://www.kernel.org/doc/Documentation/input/event-codes.txt
    int syn = 0;
    m_evt.type = OEVT_MOUSE;
    OcherMouseEvent* evt = &m_evt.mouse;
    for (unsigned int i = 0; i < n; ++i) {
        Log::debug(LOG_NAME, "type %d code %d value %d", kevt[i].type, kevt[i].code,
                kevt[i].value);
        switch (kevt[i].type) {
        case EV_SYN:
            syn = 1;
            break;
        case EV_ABS:
            if (kevt[i].code == ABS_X) {
                evt->x = kevt[i].value;
            } else if (kevt[i].code == ABS_Y) {
                evt->y = kevt[i].value;
            } else if (kevt[i].code == ABS_PRESSURE) {
                unsigned int pressure = kevt[i].value;
                if (pressure == 0)
                    evt->subtype = OEVT_MOUSE1_UP;
                else {
                    // TODO: 100 down, 101 drag
                    evt->subtype = OEVT_MOUSE1_DOWN;
                }
            }
            break;
        case EV_REL:
        default:
            ;
        }

        if (syn) {
#if 1       // TODO: Why do down vs up coordinate systems differ??
            if (evt->subtype == OEVT_MOUSE1_DOWN) {
                evt->x = 600 - evt->y;
                evt->y = evt->x;
            }
#endif
            Log::info(LOG_NAME, "mouse %u, %u %s", evt->x, evt->y,
                    evt->subtype == OEVT_MOUSE1_DOWN ? "down" : "up");
            m_loop.emitEvent(&m_evt);
        }
    }
}
