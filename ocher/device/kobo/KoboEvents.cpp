#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h>

#include "clc/support/Debug.h"
#include "clc/support/Logger.h"
#include "ocher/device/kobo/KoboEvents.h"

#define LOG_NAME "ocher.dev.KoboEvents"


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


KoboEvents::KoboEvents() :
    kevtHead(0),
    kevtTail(0)
{
    // TODO handle failures
    m_buttonFd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
    m_touchFd = open("/dev/input/event1", O_RDONLY | O_NONBLOCK);
    pipe(m_pipe);

    memset(&m_evt, 0, sizeof(m_evt));
}

KoboEvents::~KoboEvents()
{
#if 0
    write(m_pipe[1], "", 1);
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

void KoboEvents::flush()
{
    kevtHead = kevtTail = 0;
//    struct KoboButtonEvent kbe;
//    while (read(m_buttonFd, &kbe, sizeof(kbe)) > 0)
//        ;
    struct input_event evt;
    while (read(m_touchFd, &evt, sizeof(evt)) > 0)
        ;
    clc::Log::info(LOG_NAME, "flushed");
}

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

    int r;
    if (kevtHead == kevtTail) {
        r = select(nfds + 1, &rdfds, 0, 0, 0);
    } else {
        r = 1;
    }

    if (r == -1) {
        clc::Log::error(LOG_NAME, "select: %s", strerror(errno));
        return -1;
    } else if (r == 0) {
        clc::Log::error(LOG_NAME, "select: timeout");
        return -1;
    } else {
        clc::Log::debug(LOG_NAME, "select: awake");

        while (1) {
            struct KoboButtonEvent kbe;
            r = read(m_buttonFd, &kbe, sizeof(kbe));
            if (r == -1) {
                if (errno == EINTR)
                    continue;
                ASSERT(errno == EAGAIN || errno == EWOULDBLOCK);
                break;
            } else if (r == sizeof(kbe)) {
                if (kbe.button == 0x66) {
                    evt->type = OEVT_KEY;
                    evt->subtype = kbe.press ? OEVT_KEY_DOWN : OEVT_KEY_UP;
                    evt->key.key = OEVTK_HOME;
                    return 0;
                } else if (kbe.button == 0x74) {
                    evt->type = OEVT_KEY;
                    evt->subtype = kbe.press ? OEVT_KEY_DOWN : OEVT_KEY_UP;
                    evt->key.key = OEVTK_POWER;
                    return 0;
                }
            } else {
                break;
            }
        }

        // http://www.kernel.org/doc/Documentation/input/event-codes.txt
        while (1) {
            const unsigned int N = 64;
            STATIC_ASSERT(sizeof(kevt) == N*sizeof(struct input_event));

            if (kevtHead == kevtTail) {
                while (1) {
                    unsigned int head, tail, slots;
                    head = kevtHead;
                    tail = kevtTail;
                    if (tail < head)
                        tail += N;
                    slots = N - (tail - head);
                    if (tail + slots > N)
                        slots = N - tail;

                    if (slots) {
                        clc::Log::debug(LOG_NAME, "reading %u events at offset %u", slots, tail);
                        r = read(m_touchFd, &kevt[tail], sizeof(struct input_event)*slots);
                        if (r == -1) {
                            if (errno == EINTR)
                                continue;
                            ASSERT(errno == EAGAIN || errno == EWOULDBLOCK);
                            clc::Log::debug(LOG_NAME, "read 0 events");
                            break;
                        }
                        unsigned int n = r / sizeof(struct input_event);
                        clc::Log::debug(LOG_NAME, "read %u events", n);

                        kevtTail += n;
                        if (kevtTail > N)
                            kevtTail = 0;
                    }
                    break;
                }
            }

            int syn = 0;
            while (1) {
                if (kevtHead == kevtTail) {
                    kevtHead = kevtTail = 0;  // Allow maximal read next time
                    clc::Log::debug(LOG_NAME, "empty");
                    return -1;
                }

                clc::Log::info(LOG_NAME, "type %d code %d value %d", kevt[kevtHead].type,
                        kevt[kevtHead].code, kevt[kevtHead].value);
                if (kevt[kevtHead].type == EV_SYN) {
                    syn = 1;
                } else if (kevt[kevtHead].type == EV_ABS) {
                    // TODO:  orientation is unexpected; fiddle vinfo.rotate
                    m_evt.type = OEVT_MOUSE;
                    uint16_t code = kevt[kevtHead].code;
                    if (code == ABS_X) {
                        m_evt.mouse.y = kevt[kevtHead].value;
                    } else if (code == ABS_Y) {
                        m_evt.mouse.x = 600-kevt[kevtHead].value;
                    } else if (code == ABS_PRESSURE) {
                        m_evt.subtype = kevt[kevtHead].value ? OEVT_MOUSE1_DOWN : OEVT_MOUSE1_UP;
                    }
                }

                kevtHead++;
                if (kevtHead > N)
                    kevtHead = 0;

                if (syn) {
                    clc::Log::info(LOG_NAME, "mouse %u, %u %s", m_evt.mouse.x, m_evt.mouse.y,
                            m_evt.subtype == OEVT_MOUSE1_DOWN ? "down" : "up");
                    *evt = m_evt;
                    return 0;
                }
            }
        }
    }
    return -1;
}
