/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/UxControllerFb.h"

#include "Container.h"
#include "ux/fb/RendererFb.h"
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
        // XXX  This is getting called from Controller.cpp:130
        // which starts the SDL thread
        // which tries to inject an SDL event into the event loop,
        // but that's set from Controller.cpp:146
        if (frameBuffer->init()) {
            m_name += ".sdl";
            break;
        }
#endif
#ifdef OCHER_TARGET_KOBO
        frameBuffer = new FrameBufferMx50();
        if (frameBuffer->init()) {
            m_name += ".mx50";
            break;
        }
#endif
        return false;
    } while (false);

    g_container.frameBuffer = m_frameBuffer = frameBuffer;
    m_renderer = new RendererFb(m_frameBuffer);
    m_fontEngine = new FontEngine(m_frameBuffer);

    m_systemBar = new SystemBar(m_frameBuffer, g_container.battery);
    m_navBar = new NavBar(m_frameBuffer);

    m_bootActivity = new BootActivityFb(this);
    m_homeActivity = new HomeActivityFb(this);
    m_libraryActivity = new LibraryActivityFb(this);
    m_readActivity = new ReadActivityFb(this);
    m_settingsActivity = new SettingsActivityFb(this);
    m_sleepActivity = new SleepActivityFb(this);
    m_syncActivity = new SyncActivityFb(this);

    m_screen.setFrameBuffer(frameBuffer);
    m_screen.setEventLoop(g_container.loop);

    return true;
}

void UxControllerFb::setNextActivity(enum ActivityType a)
{
    Log::info(LOG_NAME, "next activity: %d", a);
    if (a == ACTIVITY_QUIT) {
        m_loop->stop();
    } else {
        if (m_activity) {
            m_screen.removeChild(m_activity);
            m_activity->onDetached();
        }

        switch (a) {
        case ACTIVITY_BOOT:
            m_activity = m_bootActivity;
            break;
        case ACTIVITY_SLEEP:
            m_activity = m_sleepActivity;
            break;
        case ACTIVITY_SYNC:
            m_activity = m_syncActivity;
            break;
        case ACTIVITY_HOME:
            m_activity = m_homeActivity;
            break;
        case ACTIVITY_READ:
            m_activity = m_readActivity;
            break;
        case ACTIVITY_LIBRARY:
            m_activity = m_libraryActivity;
            break;
        case ACTIVITY_SETTINGS:
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
