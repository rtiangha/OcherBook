#include <unistd.h>
#include <SDL/SDL.h>

#include "clc/support/Logger.h"
#include "ocher/ux/fb/sdl/SdlLoop.h"

#define LOG_NAME "ocher.sdl"

SdlLoop::SdlLoop()
{
    pipe(m_pipe);
    // TODO nonblocking
}

SdlLoop::~SdlLoop()
{
    ASSERT(!isAlive());
}

void SdlLoop::start(EventLoop* loop, clc::Monitor* monitor)
{
    ASSERT(!isAlive());
    m_loop = loop;
    m_startupMonitor = monitor;
    clc::Thread::start();
}

void SdlLoop::stop()
{
    ASSERT(isAlive());
    interrupt();
}

SDL_Surface* SdlLoop::init()
{
    SDL_Surface* screen = 0;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        const char* err = SDL_GetError();
        clc::Log::error(LOG_NAME, "SDL_Init failed: %s", err);
    } else {
        // TODO:  store window size in user settings
        screen = SDL_SetVideoMode(600, 800, 8, SDL_SWSURFACE | SDL_HWPALETTE);
        if (! screen) {
            const char* err = SDL_GetError();
            clc::Log::error(LOG_NAME, "SDL_SetVideoMode failed: %s", err);
            SDL_Quit();
        } else {
            clc::Log::debug(LOG_NAME, "SDL_Init ok");
        }
    }
    return screen;
}

void SdlLoop::run()
{
    m_screen = init();
    m_startupMonitor->lock();
    m_startupMonitor->notify();
    m_startupMonitor->unlock();
    if (! m_screen)
        return;

    SDL_Event event;

    while (! isInterrupted()) {
        if (SDL_WaitEvent(&event) == 0) {
            clc::Log::error(LOG_NAME, "SDL_WaitEvent: %s", SDL_GetError());
            continue;
        }

        OcherEvent evt;
        memset(&evt, 0, sizeof(evt));
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
                    m_loop->appEvent(&evt.app);
                }
                break;
            }
            case SDL_QUIT:
            {
                evt.type = OEVT_APP;
                evt.app.subtype = OEVT_APP_CLOSE;
                m_loop->appEvent(&evt.app);
                break;
            }
            case SDL_KEYUP:
            {
                evt.type = OEVT_KEY;
                evt.key.subtype = OEVT_KEY_UP;
                evt.key.key = event.key.keysym.sym;
                break;
            }
            case SDL_KEYDOWN:
            {
                evt.type = OEVT_KEY;
                evt.key.subtype = OEVT_KEY_DOWN;
                evt.key.key = event.key.keysym.sym;
                break;
            }
            case SDL_MOUSEMOTION:
            {
                //event.motion.x
                //event.motion.y
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
    }
}

