/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "Container.h"

#include "device/Battery.h"
#include "device/Device.h"
#include "device/Filesystem.h"
#include "settings/Options.h"
#include "settings/Settings.h"
#include "ux/Controller.h"
#include "ux/PowerSaver.h"
#include "ux/Renderer.h"
#include "ux/fb/FontEngine.h"
#include "ux/fb/FrameBuffer.h"


Container g_container;

Container::Container() :
    loop(nullptr),
    device(nullptr),
    battery(nullptr),
    filesystem(nullptr),
    settings(nullptr),
    options(nullptr),
    powerSaver(nullptr),
    uxController(nullptr),
    frameBuffer(nullptr),
    fontEngine(nullptr),
    renderer(nullptr)
{
}

Container::~Container()
{
    delete uxController;
    delete battery;
    delete filesystem;
    delete settings;
    delete powerSaver;
    delete loop;
    delete device;
#ifdef UX_FB  // TODO:  Does this belong on UxControllerFb?  no one else would use it.
    delete frameBuffer;
    delete fontEngine;
#endif
    delete renderer;

}

// wire
// unwire
// destroy
