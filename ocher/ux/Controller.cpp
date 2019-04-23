/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher_config.h"

#include "ux/Controller.h"

#include "Container.h"
#include "device/Battery.h"
#include "device/Device.h"
#include "device/Filesystem.h"
#include "settings/Options.h"
#include "settings/Settings.h"
#include "ux/PowerSaver.h"
#ifdef UX_FB
#include "ux/fb/UxControllerFb.h"
#endif
#ifdef UX_CDK
#include "ux/cdk/UxControllerCdk.h"
#endif
#ifdef UX_FD
#include "ux/fd/UxControllerFd.h"
#endif
#include "util/Debug.h"
#include "util/LogAppenders.h"
#include "util/Logger.h"

#ifdef AIRBAG_FD
#include <airbag_fd.h>
#endif

#include <chrono>
#include <stdexcept>
#include <thread>

#define LOG_NAME "ocher.controller"


UxController::UxController()
{
    g_container->filesystem.dirChanged.Connect(this, &UxController::onDirChanged);
    g_container->filesystem.initWatches(g_container->loop);

    g_container->loop.emitEvent.Connect(this, &UxController::handleEvent);
    g_container->powerSaver.wantToSleep.Connect(this, &UxController::onWantToSleep);
    g_container->powerSaver.wantToPowerOff.Connect(this, &UxController::onWantToPowerOff);
}

UxController::~UxController()
{
    g_container->filesystem.dirChanged.Disconnect(this, &UxController::onDirChanged);
    g_container->filesystem.deinitWatches(g_container->loop);

    g_container->loop.emitEvent.Disconnect(this, &UxController::handleEvent);
    g_container->powerSaver.wantToSleep.Disconnect(this, &UxController::onWantToSleep);
    g_container->powerSaver.wantToPowerOff.Disconnect(this, &UxController::onWantToPowerOff);
}

void UxController::onDirChanged(const char* dir, const char* file)
{
    Log::info(LOG_NAME, "onDirChanged %s %s", dir, file);
    // TODO
}

void UxController::onWantToSleep()
{
    Log::info(LOG_NAME, "onWantToSleep");

    setNextActivity(Activity::Type::Sleep);
}

void UxController::onWantToPowerOff()
{
    Log::info(LOG_NAME, "onWantToPowerOff");

    setNextActivity(Activity::Type::PowerOff);
}

void UxController::handleEvent(const struct OcherEvent* evt)
{
    if (evt->type == OEVT_APP && evt->app.subtype == OEVT_APP_CLOSE) {
        setNextActivity(Activity::Type::Quit);
    }
}

Controller::Controller(const Options& options)
{
    g_container = make_unique<Container>();
    g_container->options = options;

    initLog();
    initDevice();

    g_container->settings.load();

    // Sort UxController list based on desirability
    // (assume native >> toolkits >> framebuffer >> tui >> text)
#ifdef UX_FB
    if (!g_container->uxController) {
        try {
            initUxController(std::unique_ptr<UxController>(new UxControllerFb));
        } catch (...)
        {
            Log::warn(LOG_NAME, "Skipping fb driver");
        }
    }
#endif
#ifdef UX_CDK
    if (!g_container->uxController) {
        try {
            initUxController(std::unique_ptr<UxController>(new UxControllerCdk));
        } catch (...)
        {
            Log::warn(LOG_NAME, "Skipping cdk driver");
        }
    }
#endif
#ifdef UX_FD
    if (!g_container->uxController) {
        try {
            initUxController(std::unique_ptr<UxController>(new UxControllerFd));
        } catch (...)
        {
            Log::warn(LOG_NAME, "Skipping fd driver");
        }
    }
#endif
    UxController* uxController = g_container->uxController.get();
    if (!uxController) {
        throw std::runtime_error("failed to find suitable output driver");
    }
    Log::info(LOG_NAME, "Using the '%s' driver", uxController->getName().c_str());

    initCrash();
}

Controller::~Controller()
{
    // Keep g_container valid while deleting the objects in it
    auto container = g_container.get();
    delete container;
    g_container.release();
}

void Controller::initCrash()
{
    // TODO:  Could pass on to uxController...
#ifdef AIRBAG_FD
    airbag_init_fd(2, 0);
#endif
}

void Controller::initUxController(std::unique_ptr<UxController> c)
{
    Log::info(LOG_NAME, "considering driver %s", c->getName().c_str());

    if (g_container->options.driverName &&
            c->getName() != g_container->options.driverName) {
        throw std::runtime_error("skipping driver based on user pref");
    }
    if (g_container->options.listDrivers) {
        printf("\t%s\n", c->getName().c_str());
        throw std::runtime_error("skipping driver; only listing");
    }
    if (!c->init()) {
        Log::warn(LOG_NAME, "Failed to init the '%s' driver", g_container->options.driverName);
        throw std::runtime_error("failed to init driver");
    }
    g_container->uxController = std::move(c);
}

void Controller::initLog()
{
    static LogAppenderCFile appender(stderr);
    Logger* l = Log::get("");

    l->setAppender(&appender);

    Options& opt = g_container->options;
    if (opt.verbose < 0)
        l->setLevel(Log::Fatal);
    else if (opt.verbose == 0)
        l->setLevel(Log::Error);
    else if (opt.verbose == 1)
        l->setLevel(Log::Warn);
    else if (opt.verbose == 2)
        l->setLevel(Log::Info);
    else if (opt.verbose == 3)
        l->setLevel(Log::Debug);
    else
        l->setLevel(Log::Trace);
    Log::info("ocher", "Log initialized");
}

void Controller::initDevice()
{
    g_container->device = Device::create();

#if 0
    // Before proceeding with startup and initializing the framebuffer, check for a killswitch.
    // Useful when needing to bail to native stack (such as OcherBook vs native stack init-ing
    // framebuffer in incompatible ways).
    if (g_container->device->fs.m_libraries) {
        for (int i = 0;; ++i) {
            const char* lib = g_container->device->fs.m_libraries[i];
            if (!lib)
                break;
            std::string killswitch(1, "%s/.ocher/kill", lib);
            if (access(killswitch.c_str(), F_OK) == 0) {
                fprintf(stderr, "OcherBook exiting because of '%s' killswitch\n", killswitch.c_str());
                exit(1);
            }
        }
    }
#endif

#ifdef OCHER_TARGET_KOBO
    // Kobo rc scripts start animate.sh, which shows an animation while nickel is starting.
    // Kill that here (so it doesn't overlay the boot menu) to simplify installation steps.
    // TODO: Remove when more closely integrated.
    system("killall on-animator.sh");
    std::this_thread::sleep_for(std::chrono::seconds(1));
#endif
}

void Controller::run()
{
    Activity::Type a = g_container->options.bootMenu ? Activity::Type::Boot : Activity::Type::Sync;
    g_container->uxController->setNextActivity(a);

    g_container->loop.run();

    // TODO: sync state out
}
