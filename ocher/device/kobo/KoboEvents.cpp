#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h>

#include "clc/support/Debug.h"
#include "clc/support/Logger.h"

#include "ocher/device/kobo/KoboEvents.h"


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
                } else if (kevt.button == 0x74) {
                    evt->type = OEVT_KEY;
                    evt->subtype = kevt.press ? OEVT_KEY_DOWN : OEVT_KEY_UP;
                    evt->key.key = OEVTK_POWER;
                }
            } else {
                break;
            }
        }

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
                   // if (kevt[i].type == EV_SYN) {
                   //     evt->type = OEVT_MOUSE;
                   //     //kevt[i].time.tv_sec, kevt[i].time.tv_usec, kevt[i].code ? "Config Sync" : "Report Sync" );
                   // }
                }
            } else {
                break;
            }
        }
    }
    return 0;
}


