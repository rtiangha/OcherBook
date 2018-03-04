/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_CONTROLLER_H
#define OCHER_UX_CONTROLLER_H

#include "shelf/Shelf.h"
#include "ux/Activity.h"

class Device;
class EventLoop;
class Filesystem;
class FontEngine;
class FrameBuffer;
class Options;
class PowerSaver;
class Renderer;

/** Holds the current reading state.
 */
class ReadingContext {
public:
    ReadingContext() :
        shortList(&library),
        selected(0)
    {
    }

    Library library;
    ShortList shortList;

    Meta* selected;
};

/**
 * A UxController is a loose coupling between the Activities and the display mechanism used.
 *
 * The UxController is told what it should be doing (via an Activity) and in return gathers events and
 * passes them to its implementation-dependent children.
 *
 * Derive from UxController (perhaps) per toolkit.
 */
class UxController {
public:
    UxController();
    virtual ~UxController();

    virtual const char* getName() const = 0;

    virtual bool init() = 0;

    virtual FrameBuffer* getFrameBuffer()
    {
        return nullptr;
    }

    virtual FontEngine* getFontEngine()
    {
        return nullptr;
    }
    virtual Renderer* getRenderer() = 0;

    // TODO: in general, need more control (when running a new activity, is the prior
    // one destroyed?  or suspended?
    virtual void setNextActivity(enum ActivityType a) = 0;

    void onWantToSleep();
    void onDirChanged(const char* dir, const char* file);
    void handleEvent(const struct OcherEvent* evt);

    ReadingContext ctx;

protected:
    Filesystem* m_filesystem;
    PowerSaver* m_powerSaver;
    EventLoop* m_loop;
};

/**
 * Top-level controller, managing startup and shutdown.  During startup, it initializes the Container
 * and builds an appropriate UxController.  Delegates the user-interaction to a UxController.
 */
class Controller {
public:
    Controller(Options* options);
    ~Controller() = default;

    void run();

protected:
    void initCrash();
    void initLog();
    void initDebug();

    UxController* m_uxController;
};

#endif
