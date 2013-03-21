#include "clc/support/Debug.h"
#include "clc/support/Logger.h"
#include "ocher/device/Device.h"
#include "ocher/ux/Controller.h"
#include "ocher/ux/Factory.h"
#include "ocher/ux/PowerSaver.h"

#define LOG_NAME "ocher.controller"


Controller::Controller() :
    m_activity(ACTIVITY_SYNC),
    m_activityPanel(0),
    m_homeActivity(this),
    m_libraryActivity(this),
    m_readActivity(this),
    m_settingsActivity(this),
    m_syncActivity(this)
{
    // TODO:  replace with IOC
    m_powerSaver = new PowerSaver();
    m_powerSaver->setEventLoop(g_loop);

    g_fb->setEventLoop(g_loop);

    g_device->fs.dirChanged.Connect(this, &Controller::onDirChanged);
}

Controller::~Controller()
{
    delete m_powerSaver;
    g_device->fs.dirChanged.Disconnect(this, &Controller::onDirChanged);
}

void Controller::onDirChanged(const char* dir, const char* file)
{
    clc::Log::info(LOG_NAME, "onDirChanged %s %s", dir, file);
    // TODO
}

void Controller::setNextActivity(Activity a)
{
    clc::Log::info(LOG_NAME, "next activity: %d", a);
    if (m_activityPanel) {
        m_activityPanel->onDetached();
        g_loop->mouseEvent.Disconnect(m_activityPanel, &Widget::onMouseEvent);
        g_loop->keyEvent.Disconnect(m_activityPanel, &Widget::onKeyEvent);
    }

    switch (a) {
        case ACTIVITY_SYNC:
            m_activityPanel = &m_syncActivity;
            break;
        case ACTIVITY_HOME:
            m_activityPanel = &m_homeActivity;
            break;
        case ACTIVITY_READ:
            m_activityPanel = &m_readActivity;
            break;
        case ACTIVITY_LIBRARY:
            m_activityPanel = &m_libraryActivity;
            break;
        case ACTIVITY_SETTINGS:
            m_activityPanel = &m_settingsActivity;
            break;
        default:
            ASSERT(0);
    }
    g_loop->mouseEvent.Connect(m_activityPanel, &Widget::onMouseEvent);
    g_loop->keyEvent.Connect(m_activityPanel, &Widget::onKeyEvent);
    m_activityPanel->dirty();
    m_activityPanel->onAttached();
}

void Controller::run()
{
    setNextActivity(m_activity);
    g_loop->run();

    // TODO: sync state out
}
