#include "clc/support/Debug.h"
#include "clc/support/Logger.h"
#include "ocher/device/Device.h"
#include "ocher/ux/Controller.h"
#include "ocher/ux/Factory.h"
#include "ocher/ux/PowerSaver.h"

#define LOG_NAME "ocher.controller"


Controller::Controller() :
    m_activity(ACTIVITY_SYNC),
    m_homeActivity(this),
    m_libraryActivity(this),
    m_readActivity(this),
    m_settingsActivity(this),
    m_syncActivity(this)
{
    m_powerSaver = new PowerSaver();
    m_powerSaver->setEventLoop(g_loop);

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

void Controller::run()
{
    // create canvas

    g_loop->run();

    // TODO: sync state out
}
