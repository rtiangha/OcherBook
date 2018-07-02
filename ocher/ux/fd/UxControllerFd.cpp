/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fd/UxControllerFd.h"

#include "Container.h"
#include "util/Logger.h"
#include "ux/Event.h"
#include "ux/Renderer.h"
#include "ux/fd/RendererFd.h"

#define LOG_NAME "ocher.ux.ctrl"

bool UxControllerFd::init()
{
    m_renderer = make_unique<RendererFd>();

    return true;
}

void UxControllerFd::setNextActivity(Activity::Type a)
{
    Log::info(LOG_NAME, "next activity: %d", (int)a);
    if (a == Activity::Type::Quit) {
        g_container.loop.stop();
    } else {
        // TODO
    }
}
