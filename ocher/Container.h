/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_CONTAINER_H
#define OCHER_CONTAINER_H

#include <memory>

#include "device/Battery.h"
#include "device/Device.h"
#include "device/Filesystem.h"
#include "settings/Options.h"
#include "settings/Settings.h"
#include "ux/Controller.h"
#include "ux/Event.h"
#include "ux/PowerSaver.h"
#ifdef OCHER_TARGET_KOBO
#include "device/kobo/KoboEvents.h"
#endif

class Container {
public:
    Container();

    EventLoop loop;
    Filesystem filesystem;
    Settings settings;
    Options options;
    std::unique_ptr<Device> device;
    Battery battery;
    PowerSaver powerSaver;

    std::unique_ptr<UxController> uxController;

#ifdef OCHER_TARGET_KOBO
    KoboEvents koboEvents;
#endif

    // TODO event source(s) (feeds to EventLoop.  eg: SdlThread, KoboEvents, BrowseFd.cpp:getKey, ncurses, ...)
    // TODO time (common timebase for events.  needed by event loop, clock, ...)

};

extern std::unique_ptr<Container> g_container;

#endif
