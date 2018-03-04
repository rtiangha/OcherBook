/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher/ux/fd/UxControllerFd.h"

#include "ocher/ux/Event.h"
#include "ocher/ux/Renderer.h"
#include "ocher/ux/fd/RendererFd.h"
#include "ocher/util/Logger.h"

#define LOG_NAME "ocher.ux.ctrl"

UxControllerFd::UxControllerFd() :
    m_renderer(nullptr)
{
}

UxControllerFd::~UxControllerFd()
{
    delete m_renderer;
}

bool UxControllerFd::init()
{
    m_renderer = new RendererFd;

    return true;
}

void UxControllerFd::setNextActivity(enum ActivityType a)
{
    Log::info(LOG_NAME, "next activity: %d", a);
    if (a == ACTIVITY_QUIT) {
        m_loop->stop();
    } else {
        // TODO
    }
}
