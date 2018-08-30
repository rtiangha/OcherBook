/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/UxControllerFb.h"
#include "ux/fb/BootActivityFb.h"
#include "ux/fb/HomeActivityFb.h"
#include "ux/fb/LibraryActivityFb.h"
#include "ux/fb/ReadActivityFb.h"
#include "ux/fb/SettingsActivityFb.h"
#include "ux/fb/SleepActivityFb.h"
#include "ux/fb/SyncActivityFb.h"

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
    m_name("fb"),
    m_screen(g_container->loop)
{
}

bool UxControllerFb::init()
{
    FrameBuffer* frameBuffer;

    do {
#ifdef UX_FB_SDL
        frameBuffer = new FrameBufferSdl(g_container->loop);
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

    m_frameBuffer = std::unique_ptr<FrameBuffer>(frameBuffer);
    m_screen.setFrameBuffer(frameBuffer);
    m_renderer = make_unique<RendererFb>(m_frameBuffer.get());
    m_fontEngine = make_unique<FontEngine>(m_frameBuffer.get());

    return true;
}

void UxControllerFb::setNextActivity(Activity::Type a)
{
    Log::info(LOG_NAME, "next activity: %d", (int)a);
    if (a == Activity::Type::Quit) {
        g_container->loop.stop();
    } else {
        if (m_activity) {
            m_screen.removeChild(m_activity);
        }

        std::unique_ptr<ActivityFb> activity;

        switch (a) {
        case Activity::Type::Boot:
            activity.reset(new BootActivityFb(this));
            break;
        case Activity::Type::Sleep:
            activity.reset(new SleepActivityFb(this));
            break;
        case Activity::Type::Sync:
            activity.reset(new SyncActivityFb(this, g_container->filesystem));
            break;
        case Activity::Type::Home:
            activity.reset(new HomeActivityFb(this));
            break;
        case Activity::Type::Read:
            activity.reset(new ReadActivityFb(this));
            break;
        case Activity::Type::Library:
            activity.reset(new LibraryActivityFb(this));
            break;
        case Activity::Type::Settings:
            activity.reset(new SettingsActivityFb(this));
            break;
        default:
            ASSERT(0);
        }

        m_activity = activity.get();  // remember for removal
        m_screen.addChild(std::move(activity));

        m_frameBuffer->needFull();
    }
}
