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

void UxControllerCdk::setNextActivity(enum ActivityType a)
{
    Log::info(LOG_NAME, "next activity: %d", a);
    if (a == ACTIVITY_QUIT) {
        m_loop->stop();
    } else {

        // TODO:  this just sends an event to the CDK thread
        switch (a) {
        case ACTIVITY_BOOT:
            // TODO
            setNextActivity(ACTIVITY_HOME);
            break;
        case ACTIVITY_SLEEP:
            // TODO
            setNextActivity(ACTIVITY_HOME);
            break;
        case ACTIVITY_SYNC:
            // TODO
            setNextActivity(ACTIVITY_HOME);
            break;
        case ACTIVITY_HOME:
            break;
        case ACTIVITY_READ:
            break;
        case ACTIVITY_LIBRARY:
            break;
        case ACTIVITY_SETTINGS:
            // TODO
            setNextActivity(ACTIVITY_HOME);
            break;
        default:
            ASSERT(false);
        }
    }
}
