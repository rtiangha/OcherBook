/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/cdk/UxControllerCdk.h"

#include "util/Debug.h"
#include "util/Logger.h"
#include "ux/Event.h"
#include "ux/cdk/RendererCdk.h"

#define LOG_NAME "ocher.ux.cdk"

UxControllerCdk::UxControllerCdk() :
    m_scr(nullptr),
    m_screen(nullptr),
    m_renderer(nullptr)
{
}

UxControllerCdk::~UxControllerCdk()
{
    delete m_renderer;
    m_renderer = nullptr;

    destroyCDKScreen(m_screen);
    endCDK();
    delwin(m_scr);
}

bool UxControllerCdk::init()
{
    m_scr = initscr();
    m_screen = initCDKScreen(m_scr);
    initCDKColor();

    // TODO:  This should be a thread, like SDL
    m_renderer = new RendererCdk(m_scr, m_screen);

    return true;
}

void UxControllerCdk::setNextActivity(Activity::Type a)
{
    Log::info(LOG_NAME, "next activity: %d", (int)a);
    if (a == Activity::Type::Quit) {
        m_loop->stop();
    } else {

        // TODO:  this just sends an event to the CDK thread
        switch (a) {
        case Activity::Type::Boot:
            // TODO
            setNextActivity(Activity::Type::Home);
            break;
        case Activity::Type::Sleep:
            // TODO
            setNextActivity(Activity::Type::Home);
            break;
        case Activity::Type::Sync:
            // TODO
            setNextActivity(Activity::Type::Home);
            break;
        case Activity::Type::Home:
            break;
        case Activity::Type::Read:
            break;
        case Activity::Type::Library:
            break;
        case Activity::Type::Settings:
            // TODO
            setNextActivity(Activity::Type::Home);
            break;
        default:
            ASSERT(false);
        }
    }
}
