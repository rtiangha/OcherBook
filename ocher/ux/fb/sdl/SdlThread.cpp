/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher/ux/fb/sdl/SdlThread.h"
#include "ocher/util/Logger.h"

#include <SDL/SDL.h>

#define LOG_NAME "ocher.sdl"


SdlThread::SdlThread() :
    Thread("SdlThread"),
    m_loop(0),
    m_startupMonitor(0),
    m_screen(0)
{
}

SdlThread::~SdlThread()
{
    ASSERT(!isAlive());
}

void SdlThread::setEventLoop(EventLoop *loop)
{
    m_loop = loop;
}

void SdlThread::start(Monitor *monitor)
{
    ASSERT(!isAlive());
    m_startupMonitor = monitor;
    Thread::start();
}

void SdlThread::stop()
{
    interrupt();
}

SDL_Surface *SdlThread::init()
{
    SDL_Surface *screen = 0;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        const char *err = SDL_GetError();
        Log::error(LOG_NAME, "SDL_Init failed: %s", err);
    } else {
        // TODO:  store window size in user settings
        screen = SDL_SetVideoMode(600, 800, 8, SDL_SWSURFACE | SDL_HWPALETTE);
        if (!screen) {
            const char *err = SDL_GetError();
            Log::error(LOG_NAME, "SDL_SetVideoMode failed: %s", err);
            SDL_Quit();
        } else {
            Log::debug(LOG_NAME, "SDL_Init ok");
        }
    }
    return screen;
}

void SdlThread::run()
{
    m_screen = init();
    m_startupMonitor->lock();
    m_startupMonitor->notify();
    m_startupMonitor->unlock();
    if (!m_screen)
        return;

    SDL_Event event;

    while (!isInterrupted()) {
        if (SDL_WaitEvent(&event) == 0) {
            Log::error(LOG_NAME, "SDL_WaitEvent: %s", SDL_GetError());
            continue;
        }

        OcherEvent evt;
        switch (event.type) {
        case SDL_ACTIVEEVENT:
        {
            if (event.active.state & SDL_APPACTIVE) {
                evt.type = OEVT_APP;
                if (event.active.gain) {
                    evt.app.subtype = OEVT_APP_ACTIVATE;
                } else {
                    evt.app.subtype = OEVT_APP_DEACTIVATE;
                }
            }
            break;
        }
        case SDL_QUIT:
        {
            evt.type = OEVT_APP;
            evt.app.subtype = OEVT_APP_CLOSE;
            break;
        }
        case SDL_KEYUP:
        {
            evt.type = OEVT_KEY;
            evt.key.subtype = OEVT_KEY_UP;
            // Remap some keys to simulate/test dedicated hardware buttons:
            switch (event.key.keysym.sym) {
            case SDLK_POWER:          // probably caught by OS
            case SDLK_F3:
                evt.key.key = OEVTK_POWER;
                break;
            default:
                evt.key.key = event.key.keysym.sym;
            }
            break;
        }
        case SDL_KEYDOWN:
        {
            evt.type = OEVT_KEY;
            evt.key.subtype = OEVT_KEY_DOWN;
            // Remap some keys to simulate/test dedicated hardware buttons:
            switch (event.key.keysym.sym) {
            case SDLK_POWER:          // probably caught by OS
            case SDLK_F3:
                evt.key.key = OEVTK_POWER;
                break;
            default:
                evt.key.key = event.key.keysym.sym;
            }
            break;
        }
        case SDL_MOUSEMOTION:
        {
            // event.motion.x
            // event.motion.y
            break;
        }
        case SDL_MOUSEBUTTONUP:
        {
            evt.type = OEVT_MOUSE;
            evt.mouse.x = event.button.x;
            evt.mouse.y = event.button.y;
            if (event.button.button == SDL_BUTTON_LEFT) {
                evt.mouse.subtype = OEVT_MOUSE1_UP;
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        {
            evt.type = OEVT_MOUSE;
            evt.mouse.x = event.button.x;
            evt.mouse.y = event.button.y;
            if (event.button.button == SDL_BUTTON_LEFT) {
                evt.mouse.subtype = OEVT_MOUSE1_DOWN;
            }
            break;
        }
        }
        m_loop->injectEvent(evt);
    }
}
