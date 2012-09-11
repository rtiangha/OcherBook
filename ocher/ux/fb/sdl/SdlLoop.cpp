#include <SDL/SDL.h>

#include "ocher/ux/fb/sdl/SdlLoop.h"


int SdlLoop::wait(struct OcherEvent* evt)
{
    memset(evt, 0, sizeof(*evt));

    SDL_Event event;
    if (SDL_WaitEvent(&event) == 0)
        return -1;
    switch (event.type) {
        case SDL_ACTIVEEVENT:
        {
            if (event.active.state & SDL_APPACTIVE) {
                evt->type = OEVT_APP;
                if (event.active.gain) {
                    evt->subtype = OEVT_APP_ACTIVATE;
                } else {
                    evt->subtype = OEVT_APP_DEACTIVATE;
                }
            }
            break;
        }
        case SDL_KEYUP:
        {
            evt->type = OEVT_KEY;
            evt->subtype = OEVT_KEY_UP;
            evt->key.key = event.key.keysym.sym;  // TODO map?
            break;
        }
        case SDL_KEYDOWN:
        {
            evt->type = OEVT_KEY;
            evt->subtype = OEVT_KEY_DOWN;
            evt->key.key = event.key.keysym.sym;  // TODO map?
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
            evt->type = OEVT_MOUSE;
            evt->mouse.x = event.button.x;
            evt->mouse.y = event.button.y;
            if (event.button.button == SDL_BUTTON_LEFT) {
                evt->subtype = OEVT_MOUSE1_UP;
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        {
            evt->type = OEVT_MOUSE;
            evt->mouse.x = event.button.x;
            evt->mouse.y = event.button.y;
            if (event.button.button == SDL_BUTTON_LEFT) {
                evt->subtype = OEVT_MOUSE1_CLICKED;  // TODO hack... just follow SDL
            }
            break;
        }
        case SDL_QUIT:
        {
            evt->type = OEVT_APP;
            evt->subtype = OEVT_APP_CLOSE;
            break;
        }
    }
    return 0;
}

