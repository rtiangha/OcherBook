/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fd/UxControllerFd.h"

#include "ux/Event.h"
#include "ux/Renderer.h"
#include "ux/fd/RendererFd.h"
#include "util/Logger.h"

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

void UxControllerFd::setNextActivity(Activity::Type a)
{
    Log::info(LOG_NAME, "next activity: %d", (int)a);
    if (a == Activity::Type::Quit) {
        m_loop->stop();
    } else {
        // TODO
    }
}
