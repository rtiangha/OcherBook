/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "Container.h"
#include "settings/Settings.h"
#include "util/Logger.h"
#include "ux/fb/SettingsActivityFb.h"
#include "ux/fb/UxControllerFb.h"

#define LOG_NAME "ocher.ux.Settings"

SettingsActivityFb::SettingsActivityFb(UxControllerFb* c) :
    ActivityFb(c)
{
    // TODO widget packing
}

void SettingsActivityFb::onAttached()
{
    Log::info(LOG_NAME, "attached");

    // be friendly to devs and reload from disk
    g_container->settings.load();
}

void SettingsActivityFb::onDetached()
{
    Log::info(LOG_NAME, "detached");

    // save changes
    g_container->settings.save();
}

void SettingsActivityFb::drawBg(Rect* rect)
{
    // dim existing screen
    m_fb->byLine(rect, dim);

    int borderX = m_rect.w / 12;
    int borderY = m_rect.h / 12;
    rect->inset((borderX + borderY) / 2);
    m_fb->setFg(0, 0, 0);
    m_fb->rect(rect);

    rect->inset(1);
    m_fb->setFg(0xff, 0xff, 0xff);
    m_fb->fillRect(rect);
}
