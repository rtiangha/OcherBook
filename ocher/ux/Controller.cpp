#include "mxml.h"

#include "ocher/ux/Controller.h"
#include "ocher/ux/Factory.h"


Controller::Controller() :
    m_systemBar(m_battery)
{
}

void Controller::run()
{
    Activity a = ACTIVITY_SYNC;

    do {
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
        }
    } while (a != ACTIVITY_QUIT);

    // TODO: sync state out
}

