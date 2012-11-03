#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h>

#include "clc/support/Debug.h"
#include "clc/support/Logger.h"

#include "ocher/device/kobo/KoboEvents.h"


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


KoboEvents::KoboEvents()
{
    // TODO handle failures
    m_buttonFd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
    m_touchFd = open("/dev/input/event1", O_RDONLY | O_NONBLOCK);
    pipe(m_pipe);
}

KoboEvents::~KoboEvents()
{
#if 0
    interrupt();
    write(m_pipe[1], "", 1);
    join();
#endif
    close(m_pipe[0]);
    close(m_pipe[1]);

    if (m_buttonFd != -1) {
        close(m_buttonFd);
    }
    if (m_touchFd != -1) {
        close(m_touchFd);
    }
}

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

int KoboEvents::wait(struct OcherEvent* evt)
{
    memset(evt, 0, sizeof(*evt));

    int nfds = 0;
    fd_set rdfds;
    FD_ZERO(&rdfds);
    FD_SET(m_buttonFd, &rdfds);
    if (nfds < m_buttonFd)
        nfds = m_buttonFd;
    FD_SET(m_touchFd, &rdfds);
    if (nfds < m_touchFd)
        nfds = m_touchFd;
    FD_SET(m_pipe[0], &rdfds);
    if (nfds < m_pipe[0])
        nfds = m_pipe[0];

    int r = select(nfds + 1, &rdfds, 0, 0, 0);
    if (r == -1) {
        clc::Log::error("ocher.kobo", "select: %s", strerror(errno));
        return -1;
    } else if (r == 0) {
        clc::Log::error("ocher.kobo", "select: timeout");
        return -1;
    } else {
        clc::Log::debug("ocher.kobo", "select: awake");

        while (1) {
            struct KoboButtonEvent kevt;
            r = read(m_buttonFd, &kevt, sizeof(kevt));
            if (r == -1) {
                if (errno == EINTR)
                    continue;
                ASSERT(errno == EAGAIN || errno == EWOULDBLOCK);
                break;
            } else if (r == sizeof(kevt)) {
                if (kevt.button == 0x66) {
                    evt->type = OEVT_KEY;
                    evt->subtype = kevt.press ? OEVT_KEY_DOWN : OEVT_KEY_UP;
                    evt->key.key = OEVTK_HOME;
                    return 0;
                } else if (kevt.button == 0x74) {
                    evt->type = OEVT_KEY;
                    evt->subtype = kevt.press ? OEVT_KEY_DOWN : OEVT_KEY_UP;
                    evt->key.key = OEVTK_POWER;
                    return 0;
                }
            } else {
                break;
            }
        }

        // http://www.kernel.org/doc/Documentation/input/event-codes.txt
        while (1) {
            struct input_event kevt[64];
            r = read(m_touchFd, &kevt, sizeof(struct input_event)*64);
            if (r == -1) {
                if (errno == EINTR)
                    continue;
                ASSERT(errno == EAGAIN || errno == EWOULDBLOCK);
                break;
            } else if (r >= (int)sizeof(struct input_event)) {
                for (int i = 0; i < r / (int)sizeof(struct input_event); i++) {
                    clc::Log::info("ocher.kobo", "type %d code %d value %d", kevt[i].type,
                            kevt[i].code, kevt[i].value);
                    if (kevt[i].type == EV_SYN) {
                        return 0;
                    } else if (kevt[i].type == EV_ABS) {
                        evt->type = OEVT_MOUSE;
                        uint16_t code = kevt[i].code;
                        if (code == ABS_X) {
                            evt->mouse.x = kevt[i].value;
                        } else if (code == ABS_Y) {
                            evt->mouse.y = kevt[i].value;
                        } else if (code == ABS_PRESSURE) {
                            evt->subtype = kevt[i].value ? OEVT_MOUSE1_DOWN : OEVT_MOUSE1_UP;
                        }
                    }
                }
            } else {
                break;
            }
        }
    }
    return -1;
}


