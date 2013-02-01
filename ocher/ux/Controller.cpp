#include "clc/support/Debug.h"
#include "ocher/ux/Controller.h"
#include "ocher/ux/Factory.h"


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

    while (1) {
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
                ASSERT(0);
                break;
        }

        if (a == ACTIVITY_PREVIOUS)
            a = prev;
        else if (a == ACTIVITY_QUIT)
            break;
    }

    // TODO: sync state out
}
