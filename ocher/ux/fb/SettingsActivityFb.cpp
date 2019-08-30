/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "settings/Settings.h"
#include "util/Logger.h"
#include "ux/fb/SettingsActivityFb.h"
#include "ux/fb/UxControllerFb.h"

#define LOG_NAME "ocher.ux.Settings"

SettingsActivityFb::SettingsActivityFb(UxControllerFb* c) :
    ActivityFb(c),
    m_fb(c->getFrameBuffer())
{
}

void SettingsActivityFb::onAttached()
{
    Log::info(LOG_NAME, "attached");

    m_fb->byLine(&m_fb->bbox, dim);

    // load
}

void SettingsActivityFb::onDetached()
{
    Log::info(LOG_NAME, "detached");

    // save
}
