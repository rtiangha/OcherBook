/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_CONTAINER_H
#define OCHER_CONTAINER_H

#include <vector>

class Battery;
class Device;
class EventLoop;
class Filesystem;
class FontEngine;
class FrameBuffer;
class Options;
class PowerSaver;
class Renderer;
class Settings;
class UxController;

/**
 * A container for instances.
 *
 * This is trivial dependency injection.  The dependencies are hardcoded.  The Container owns the
 * instances.  Each instance is a singleton.
 */
class Container {
public:
    Container();
    ~Container();

    /**
     * The list of all user experience drivers.
     */
    std::vector<UxController*> uxControllers;

    /**
     * Requires:
     */
    EventLoop* loop;

    /**
     * Requires: EventLoop
     */
    Device* device;

    /**
     * Requires:
     */
    Battery* battery;

    /**
     * Requires:
     */
    Filesystem* filesystem;

    /**
     * Requires: Filesystem
     */
    Settings* settings;

    Options* options;

    /**
     * Requires: EventLoop Device
     */
    PowerSaver* powerSaver;

    /**
     */
    UxController* uxController;

    /**
     * May not exist, depending on Device.
     * May not actually be used, depending on UxController.
     *
     * Requires:
     */
    FrameBuffer* frameBuffer;

    /**
     * May not exist, depending on Device.
     * May not actually be used, depending on UxController.
     *
     * Requires:
     *  - Framebuffer
     */
    FontEngine* fontEngine;

    /**
     * Requires:
     *  - UxController
     */
    Renderer* renderer;

    // TODO event source(s) (feeds to EventLoop.  eg: SdlThread, KoboEvents, BrowseFd.cpp:getKey, ncurses, ...)
    // TODO time (common timebase for events.  needed by event loop, clock, ...)
};

extern Container g_container;

#endif
