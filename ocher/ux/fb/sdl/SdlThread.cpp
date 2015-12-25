/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher/ux/fb/sdl/SdlThread.h"
#include "ocher/util/Logger.h"

#include <SDL/SDL.h>

#include <fcntl.h>
#include <unistd.h>

#define LOG_NAME "ocher.sdl"


SdlThread::SdlThread() :
    Thread("SdlThread"),
    m_loop(0),
    m_startupMonitor(0),
    m_screen(0)
{
    pipe(m_pipe);

    int flags = fcntl(m_pipe[0], F_GETFL, 0);
    fcntl(m_pipe[0], F_SETFL, flags | O_NONBLOCK);
}

SdlThread::~SdlThread()
{
    ASSERT(!isAlive());
    close(m_pipe[0]);
    close(m_pipe[1]);
}

void SdlThread::setEventLoop(EventLoop *loop)
{
    m_loop = loop;

    m_evtWatcher.data = this;
    ev_io_init(&m_evtWatcher, fireEventsCb, m_pipe[0], EV_READ);
    ev_io_start(m_loop->evLoop, &m_evtWatcher);
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

void SdlThread::fireEventsCb(struct ev_loop *, ev_io *watcher, int)
{
    static_cast<SdlThread *>(watcher->data)->fireEvents();
}

void SdlThread::fireEvents()
{
    while (1) {
        char c;
        OcherEvent *evt = NULL;

        m_evtLock.lock();
        read(m_pipe[0], &c, 1);
        if (!m_evtQ.empty()) {
            evt = m_evtQ.front();
            m_evtQ.pop_front();
        }
        m_evtLock.unlock();
        if (!evt)
            break;
        switch (evt->type) {
        case OEVT_KEY:
            Log::debug(LOG_NAME, "keyEvent");
            m_loop->keyEvent(&evt->key);
            break;
        case OEVT_MOUSE:
            Log::debug(LOG_NAME, "mouseEvent");
            m_loop->mouseEvent(&evt->mouse);
            break;
        case OEVT_APP:
            Log::debug(LOG_NAME, "appEvent");
            m_loop->appEvent(&evt->app);
            break;
        case OEVT_DEVICE:
            Log::debug(LOG_NAME, "deviceEvent");
            m_loop->deviceEvent(&evt->device);
            break;
        }
        ;
        delete evt;
    }
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

        OcherEvent *evt = new OcherEvent;
        switch (event.type) {
        case SDL_ACTIVEEVENT:
        {
            if (event.active.state & SDL_APPACTIVE) {
                evt->type = OEVT_APP;
                if (event.active.gain) {
                    evt->app.subtype = OEVT_APP_ACTIVATE;
                } else {
                    evt->app.subtype = OEVT_APP_DEACTIVATE;
                }
            }
            break;
        }
        case SDL_QUIT:
        {
            evt->type = OEVT_APP;
            evt->app.subtype = OEVT_APP_CLOSE;
            break;
        }
        case SDL_KEYUP:
        {
            evt->type = OEVT_KEY;
            evt->key.subtype = OEVT_KEY_UP;
            // Remap some keys to simulate/test dedicated hardware buttons:
            switch (event.key.keysym.sym) {
            case SDLK_POWER:          // probably caught by OS
            case SDLK_F3:
                evt->key.key = OEVTK_POWER;
                break;
            default:
                evt->key.key = event.key.keysym.sym;
            }
            break;
        }
        case SDL_KEYDOWN:
        {
            evt->type = OEVT_KEY;
            evt->key.subtype = OEVT_KEY_DOWN;
            // Remap some keys to simulate/test dedicated hardware buttons:
            switch (event.key.keysym.sym) {
            case SDLK_POWER:          // probably caught by OS
            case SDLK_F3:
                evt->key.key = OEVTK_POWER;
                break;
            default:
                evt->key.key = event.key.keysym.sym;
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
            evt->type = OEVT_MOUSE;
            evt->mouse.x = event.button.x;
            evt->mouse.y = event.button.y;
            if (event.button.button == SDL_BUTTON_LEFT) {
                evt->mouse.subtype = OEVT_MOUSE1_UP;
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        {
            evt->type = OEVT_MOUSE;
            evt->mouse.x = event.button.x;
            evt->mouse.y = event.button.y;
            if (event.button.button == SDL_BUTTON_LEFT) {
                evt->mouse.subtype = OEVT_MOUSE1_DOWN;
            }
            break;
        }
        }
        m_evtLock.lock();
        write(m_pipe[1], "", 1);
        m_evtQ.push_back(evt);
        m_evtLock.unlock();
    }
}
