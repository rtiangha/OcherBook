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


/*
   Processor       : ARMv7 Processor rev 5 (v7l)
   BogoMIPS        : 159.90
   Features        : swp half thumb fastmult vfp edsp neon vfpv3
   CPU implementer : 0x41
   CPU architecture: 7
   CPU variant     : 0x2
   CPU part        : 0xc08
   CPU revision    : 5

   Hardware        : Freescale MX50 ARM2 Board
   Revision        : 50011
   Serial          : 0000000000000000
 */


struct KoboButtonEvent {
    uint16_t timeHigh;
    uint16_t res1;
    uint16_t timeLow;
    uint16_t res2;
    uint16_t res3;
    uint16_t button;
    uint16_t press;
    uint16_t res4;
};

KoboEvents::KoboEvents() :
    m_loop(nullptr)
{
    m_buttonFd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
    m_touchFd = open("/dev/input/event1", O_RDONLY | O_NONBLOCK);
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

void KoboEvents::setEventLoop(EventLoop* loop)
{
    m_loop = loop;

    if (m_buttonFd != -1) {
        ev_io_init(&m_buttonWatcher, buttonCb, m_buttonFd, EV_READ);
        ev_io_start(m_loop->evLoop, &m_buttonWatcher);
        m_buttonWatcher.data = this;
    }
    if (m_touchFd != -1) {
        ev_io_init(&m_touchWatcher, touchCb, m_touchFd, EV_READ);
        ev_io_start(m_loop->evLoop, &m_touchWatcher);
        m_touchWatcher.data = this;
    }
}

void KoboEvents::buttonCb(struct ev_loop* , ev_io* watcher, int)
{
    static_cast<KoboEvents*>(watcher->data)->pollButton();
}

void KoboEvents::pollButton()
{
    while (true) {
        struct KoboButtonEvent kbe;
        int r = read(m_buttonFd, &kbe, sizeof(kbe));
        if (r == -1) {
            if (errno == EINTR)
                continue;
            ASSERT(errno == EAGAIN || errno == EWOULDBLOCK);
            break;
        } else if (r == sizeof(kbe)) {
            bool fire = true;
            OcherEvent evt;
            evt.type = OEVT_KEY;
            Log::debug(LOG_NAME, "button type %x", kbe.button);
            if (kbe.button == 0x66) {
                evt.key.subtype = kbe.press ? OEVT_KEY_DOWN : OEVT_KEY_UP;
                evt.key.key = OEVTK_HOME;
            } else if (kbe.button == 0x74) {
                evt.key.subtype = kbe.press ? OEVT_KEY_DOWN : OEVT_KEY_UP;
                evt.key.key = OEVTK_POWER;
            } else {
                fire = false;
            }
            if (fire) {
                m_loop->emitEvent(&evt);
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
    Log::debug(LOG_NAME, "read %u events", n);

    // http://www.kernel.org/doc/Documentation/input/event-codes.txt
    int syn = 0;
    m_evt.type = OEVT_MOUSE;
    OcherMouseEvent* evt = &m_evt.mouse;
    for (unsigned int i = 0; i < n; ++i) {
        Log::debug(LOG_NAME, "type %d code %d value %d", kevt[i].type, kevt[i].code,
                kevt[i].value);
        if (kevt[i].type == EV_SYN) {
            syn = 1;
        } else if (kevt[i].type == EV_ABS) {
            uint16_t code = kevt[i].code;
            if (code == ABS_X) {
                evt->x = kevt[i].value;
            } else if (code == ABS_Y) {
                evt->y = kevt[i].value;
            } else if (code == ABS_PRESSURE) {
                unsigned int pressure = kevt[i].value;
                if (pressure == 0)
                    evt->subtype = OEVT_MOUSE1_UP;
                else {
                    // TODO: 100 down, 101 drag
                    evt->subtype = OEVT_MOUSE1_DOWN;
                }
            }
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
            m_loop->emitEvent(&m_evt);
        }
    }
}
