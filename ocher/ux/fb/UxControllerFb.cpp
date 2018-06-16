/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/UxControllerFb.h"

#include "Container.h"
#include "ux/fb/RendererFb.h"
#ifdef UX_FB_MX50
#include "ux/fb/mx50/FrameBufferMx50.h"
#endif
#ifdef UX_FB_SDL
#include "ux/fb/sdl/FrameBufferSdl.h"
#endif
#include "util/Debug.h"
#include "util/Logger.h"

#define LOG_NAME "ocher.ux.ctrl"

UxControllerFb::UxControllerFb() :
    m_systemBar(nullptr),
    m_navBar(nullptr),
    m_activity(nullptr),
    m_bootActivity(nullptr),
    m_homeActivity(nullptr),
    m_libraryActivity(nullptr),
    m_readActivity(nullptr),
    m_settingsActivity(nullptr),
    m_sleepActivity(nullptr),
    m_syncActivity(nullptr),
    m_renderer(nullptr),
    m_fontEngine(nullptr),
    m_frameBuffer(nullptr),
    m_name("fb")
{
}

UxControllerFb::~UxControllerFb()
{
    delete m_bootActivity;
    delete m_homeActivity;
    delete m_libraryActivity;
    delete m_readActivity;
    delete m_settingsActivity;
    delete m_sleepActivity;
    delete m_syncActivity;

    delete m_systemBar;
    delete m_navBar;

    delete m_renderer;
    delete m_fontEngine;
    delete m_frameBuffer;

}

bool UxControllerFb::init()
{
    FrameBuffer* frameBuffer;

    do {
#ifdef UX_FB_SDL
        frameBuffer = new FrameBufferSdl();
        if (frameBuffer->init()) {
            m_name += ".sdl";
            break;
        }
#endif
#ifdef UX_FB_MX50
        frameBuffer = new FrameBufferMx50();
        if (frameBuffer->init()) {
            m_name += ".mx50";
            break;
        }
#endif
        return false;
    } while (false);

    g_container.frameBuffer = m_frameBuffer = frameBuffer;
    m_screen.setFrameBuffer(frameBuffer);
    m_screen.setEventLoop(g_container.loop);
    m_renderer = new RendererFb(m_frameBuffer);
    m_fontEngine = new FontEngine(m_frameBuffer);

    m_systemBar = new SystemBar(g_container.battery);
    m_navBar = new NavBar();

    m_bootActivity = new BootActivityFb(this);
    m_homeActivity = new HomeActivityFb(this);
    m_libraryActivity = new LibraryActivityFb(this);
    m_readActivity = new ReadActivityFb(this);
    m_settingsActivity = new SettingsActivityFb(this);
    m_sleepActivity = new SleepActivityFb(this);
    m_syncActivity = new SyncActivityFb(this);

    return true;
}

void UxControllerFb::setNextActivity(Activity::Type a)
{
    Log::info(LOG_NAME, "next activity: %d", (int)a);
    if (a == Activity::Type::Quit) {
        m_loop->stop();
    } else {
        if (m_activity) {
            m_screen.removeChild(m_activity);
            m_activity->onDetached();
        }

        switch (a) {
        case Activity::Type::Boot:
            m_activity = m_bootActivity;
            break;
        case Activity::Type::Sleep:
            m_activity = m_sleepActivity;
            break;
        case Activity::Type::Sync:
            m_activity = m_syncActivity;
            break;
        case Activity::Type::Home:
            m_activity = m_homeActivity;
            break;
        case Activity::Type::Read:
            m_activity = m_readActivity;
            break;
        case Activity::Type::Library:
            m_activity = m_libraryActivity;
            break;
        case Activity::Type::Settings:
            m_activity = m_settingsActivity;
            break;
        default:
            ASSERT(0);
        }

        m_screen.addChild(m_activity);
        m_activity->onAttached();

        m_frameBuffer->needFull();
    }
}
