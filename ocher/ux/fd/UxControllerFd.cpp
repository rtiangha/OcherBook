/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher/ux/Renderer.h"
#include "ocher/ux/fd/RendererFd.h"
#include "ocher/ux/fd/UxControllerFd.h"
#include "ocher/util/Logger.h"

#define LOG_NAME ""
// TODO

UxControllerFd::UxControllerFd() :
    m_renderer(0)
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

void UxControllerFd::run(enum ActivityType a)
{
    while (m_nextActivity != ACTIVITY_QUIT) {
        Log::info(LOG_NAME, "next activity: %d", m_nextActivity);
        // TODO
    }
}
