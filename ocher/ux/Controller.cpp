#include "clc/support/Debug.h"
#include "clc/support/Logger.h"
#include "ocher/ux/Controller.h"
#include "ocher/ux/Factory.h"

#define LOG_NAME "ocher.controller"


Controller::Controller() :
    m_homeActivity(this),
    m_libraryActivity(this),
    m_readActivity(this),
    m_settingsActivity(this),
    m_syncActivity(this)
{
}

void Controller::run()
{
    Activity a = ACTIVITY_SYNC;

    do {
        Activity prev = a;

        switch (a) {
            case ACTIVITY_SYNC:
                a = m_syncActivity.run();
                break;
            case ACTIVITY_HOME:
                a = m_homeActivity.run();
                break;
            case ACTIVITY_READ:
                a = m_readActivity.run();
                break;
            case ACTIVITY_LIBRARY:
                a = m_libraryActivity.run();
                break;
            case ACTIVITY_SETTINGS:
                a = m_settingsActivity.run();
                break;
            default:
                a = prev;  // Error?
                break;
        }

        if (a == ACTIVITY_PREVIOUS)
            a = prev;
        clc::Log::info(LOG_NAME, "state %d -> %d", prev, a);
    } while (a != ACTIVITY_QUIT);

    // TODO: sync state out
}
