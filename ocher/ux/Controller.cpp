/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

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

#include "airbag_fd/airbag_fd.h"

#include <stdexcept>

#define LOG_NAME "ocher.controller"


UxController::UxController() :
    m_powerSaver(nullptr),
    m_loop(nullptr)
{
    m_filesystem = g_container.filesystem;
    m_loop = g_container.loop;
    m_powerSaver = g_container.powerSaver;

    m_filesystem->dirChanged.Connect(this, &UxController::onDirChanged);
    m_filesystem->initWatches(g_container.options);

    m_loop->emitEvent.Connect(this, &UxController::handleEvent);
    m_powerSaver->wantToSleep.Connect(this, &UxController::onWantToSleep);
}

UxController::~UxController()
{
    m_filesystem->dirChanged.Disconnect(this, &UxController::onDirChanged);
    m_loop->emitEvent.Disconnect(this, &UxController::handleEvent);
    m_powerSaver->wantToSleep.Disconnect(this, &UxController::onWantToSleep);
}

void UxController::onDirChanged(const char* dir, const char* file)
{
    Log::info(LOG_NAME, "onDirChanged %s %s", dir, file);
    // TODO
}

void UxController::onWantToSleep()
{
    Log::info(LOG_NAME, "onWantToSleep");

    // TODO
}

void UxController::handleEvent(const struct OcherEvent* evt)
{
    if (evt->type == OEVT_APP && evt->app.subtype == OEVT_APP_CLOSE) {
        setNextActivity(ACTIVITY_QUIT);
    }
}

Controller::Controller(Options* options)
    : m_uxController(nullptr)
{
    g_container.options = options;
    initLog();

    g_container.settings = new Settings();
    g_container.filesystem = new Filesystem();
    g_container.powerSaver = new PowerSaver();
    g_container.device = new Device();
    g_container.battery = new Battery();
    g_container.loop = new EventLoop();

    // Wire up
    //   TODO setters vs constructors vs hitting g_container
    //   TODO here or in Container?
    g_container.settings->inject(g_container.filesystem);
    g_container.powerSaver->inject(g_container.device);
    g_container.powerSaver->inject(g_container.loop);

    initDebug();

    g_container.settings->load();

    // Sort UxController list based on desirability
    // (assume native >> toolkits >> framebuffer >> tui >> text)
#ifdef UX_FB
    g_container.uxControllers.push_back(new UxControllerFb);
#endif
#ifdef UX_CDK
    g_container.uxControllers.push_back(new UxControllerCdk);
#endif
#ifdef UX_FD
    g_container.uxControllers.push_back(new UxControllerFd);
#endif

    // TODO....  move to run() or init()

    UxController* uxController = nullptr;
    for (UxController* c : g_container.uxControllers) {
        Log::info("ocher", "considering driver %s", c->getName());

        if (g_container.options->driverName) {
            if (strcmp(c->getName(), g_container.options->driverName) == 0) {
                Log::debug("ocher", "Attempting to init the '%s' driver", g_container.options->driverName);
                if (!c->init()) {
                    Log::warn("ocher", "Failed to init the '%s' driver", g_container.options->driverName);
                    throw std::runtime_error("failed to init driver"); // TODO
                }
                uxController = c;
                break;
            }
        } else {
            if (c->init() == true) {
                uxController = c;
                break;
            } else {
                Log::info("ocher", "driver %s failed to init", c->getName());
            }
        }
    }

    if (uxController == nullptr) {
        throw std::runtime_error("failed to find suitable output driver"); // TODO
    }
    Log::info("ocher", "Using the '%s' driver", uxController->getName());

    g_container.uxController = uxController;
#ifdef UX_FB
    g_container.frameBuffer = uxController->getFrameBuffer();
    if (g_container.frameBuffer)
        g_container.frameBuffer->inject(g_container.loop);
#endif
    g_container.fontEngine = uxController->getFontEngine();
    g_container.renderer = uxController->getRenderer();

    m_uxController = g_container.uxController;

    Log::info("ocher", "Done wiring the '%s' driver", uxController->getName());

    initCrash();
}

void Controller::initCrash()
{
    // TODO:  Could pass on to m_uxController...
#ifdef OCHER_AIRBAG_FD
    airbag_init_fd(2, 0);
#endif
}

void Controller::initLog()
{
    static LogAppenderCFile appender(stderr);
    Logger* l = Log::get("");

    l->setAppender(&appender);

    Options* opt = g_container.options;
    if (opt->verbose < 0)
        l->setLevel(Log::Fatal);
    else if (opt->verbose == 0)
        l->setLevel(Log::Error);
    else if (opt->verbose == 1)
        l->setLevel(Log::Warn);
    else if (opt->verbose == 2)
        l->setLevel(Log::Info);
    else if (opt->verbose == 3)
        l->setLevel(Log::Debug);
    else
        l->setLevel(Log::Trace);
    Log::info("ocher", "Log initialized");
}

void Controller::initDebug()
{
#if 0
    // Before proceeding with startup and initializing the framebuffer, check for a killswitch.
    // Useful when needing to bail to native stack (such as OcherBook vs native stack init-ing
    // framebuffer in incompatible ways).
    if (g_container.device->fs.m_libraries) {
        for (int i = 0;; ++i) {
            const char* lib = g_container.device->fs.m_libraries[i];
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
}

void Controller::run()
{
#ifdef OCHER_TARGET_KOBO
    // Kobo rc scripts start animate.sh, which shows an animation while nickel is starting.
    // Kill that here (so it doesn't overlay the boot menu) to simplify installation steps.
    // TODO: Remove when more closely integrated.
    system("killall on-animator.sh");
    sleep(1);
#endif

    ActivityType a = g_container.options->bootMenu ? ACTIVITY_BOOT : ACTIVITY_SYNC;
    m_uxController->setNextActivity(a);
    g_container.loop->run();

    // TODO: sync state out
}
