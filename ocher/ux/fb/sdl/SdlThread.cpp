/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher/ux/fb/sdl/SdlThread.h"

#include "ocher/util/Logger.h"

#include <SDL.h>

#define LOG_NAME "ocher.sdl"


SdlThread::SdlThread() :
    m_stop(false),
    m_loop(nullptr)
{
}

SdlThread::~SdlThread()
{
    if (m_thread.joinable())
        m_thread.join();
}

void SdlThread::setEventLoop(EventLoop* loop)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_loop = loop;
    m_cond.notify_all();
}

void SdlThread::start(std::promise<SDL_Surface*>& screenPromise)
{
    m_screenPromise = std::move(screenPromise);
    std::thread t(&SdlThread::run, this);
    m_thread = std::move(t);
}

void SdlThread::stop()
{
    m_stop = true;
    if (m_thread.joinable())
        m_thread.join();
}

SDL_Surface* SdlThread::init()
{
    SDL_Surface* screen = nullptr;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        const char* err = SDL_GetError();
        Log::error(LOG_NAME, "SDL_Init failed: %s", err);
    } else {
        // TODO:  store window size in user settings
        screen = SDL_SetVideoMode(600, 800, 8, SDL_SWSURFACE | SDL_HWPALETTE);
        if (!screen) {
            const char* err = SDL_GetError();
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
    SDL_Surface* screen = init();
    m_screenPromise.set_value(screen);
    if (!screen) {
        return;
    } else {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_loop == nullptr)
            m_cond.wait(lock);
    }

    SDL_Event event;
    while (!m_stop) {
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
                    Log::info(LOG_NAME, "SDL app active");
                    evt.app.subtype = OEVT_APP_ACTIVATE;
                } else {
                    Log::info(LOG_NAME, "SDL app inactive");
                    evt.app.subtype = OEVT_APP_DEACTIVATE;
                }
            }
            break;
        }
        case SDL_QUIT:
        {
            Log::info(LOG_NAME, "SDL quit");
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
        Log::trace(LOG_NAME, "SDL injecting event");
        m_loop->injectEvent(evt);
    }
    Log::info(LOG_NAME, "SdlThread exiting");
}
