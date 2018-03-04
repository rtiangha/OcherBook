/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/SleepActivityFb.h"

#include "Container.h"
#include "device/Device.h"
#include "ux/fb/FontEngine.h"
#include "ux/fb/UxControllerFb.h"

#include <unistd.h>

SleepActivityFb::SleepActivityFb(UxControllerFb* c) :
    ActivityFb(c),
    m_fb(c->getFrameBuffer()),
    m_device(g_container.device),
    m_loop(g_container.loop)
{
}

void SleepActivityFb::onAttached()
{
    maximize();

    m_fb->setFg(0xff, 0xff, 0xff);
    m_fb->fillRect(&m_rect);
    m_fb->setFg(0, 0, 0);

    FontEngine fe(m_fb);
    fe.setSize(18);
    fe.setItalic(1);
    fe.apply();

    Pos p;
    p.x = 0;
    p.y = m_rect.h / 2;
    fe.renderString("Sleeping", 8, &p, &m_rect, FE_XCENTER);

    m_fb->update(&m_rect, true); // Full refresh to clear page remnants
    m_fb->sync();
    ::sleep(1);                  // TODO seems hackish but sync doesn't wait long enough!

    m_device->sleep();

    m_loop->setEpoch();  // Toss old events
}

void SleepActivityFb::onDetached()
{
}
