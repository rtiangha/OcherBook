#include <exception>
#include "SDL/SDL.h"

#include "clc/support/Logger.h"
#include "clc/algorithm/Bitops.h"

#include "ocher/ux/fb/sdl/FbSdl.h"


// Ocher's Rect is chosen to match SDL's.
#define TO_SDL_RECT(r) ((SDL_Rect*)(r))


FbSdl::FbSdl() :
    m_sdl(0),
    m_screen(0)
{
}

FbSdl::~FbSdl()
{
    if (m_sdl)
        SDL_Quit();
}

bool FbSdl::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        const char *err = SDL_GetError();
        clc::Log::error("ocher.sdl", "SDL_Init failed: %s", err);
        return false;
    }

    // TODO:  store window size in user settings
    m_screen = SDL_SetVideoMode(600, 800, 8, SDL_SWSURFACE);
    if (! m_screen) {
        const char *err = SDL_GetError();
        clc::Log::error("ocher.sdl", "SDL_SetVideoMode failed: %s", err);
        SDL_Quit();
        return false;
    }
    m_mustLock = SDL_MUSTLOCK(m_screen);
    setBg(0xff, 0xff, 0xff);
    clear();
    m_sdl = 1;
    return true;
}

uint8_t FbSdl::getColor(uint8_t r, uint8_t b, uint8_t g)
{
    //return (((uint16_t)r*61)+((uint16_t)g*174)+((uint16_t)b*21))>>8;
    g = ((uint32_t)r + (uint32_t)b + (uint32_t)g) / 3;
    return SDL_MapRGB(m_screen->format, g, g, g);
}

void FbSdl::setFg(uint8_t r, uint8_t b, uint8_t g)
{
    m_fgColor = getColor(r, b, g);
}

void FbSdl::setBg(uint8_t r, uint8_t b, uint8_t g)
{
    m_bgColor = getColor(r, b, g);
}

inline unsigned int FbSdl::height()
{
    return m_screen->h;
}

inline unsigned int FbSdl::width()
{
    return m_screen->w;
}

unsigned int FbSdl::dpi()
{
    return 120;  // TODO
}

void FbSdl::clear()
{
    clc::Log::debug("ocher.sdl", "clear");
    SDL_FillRect(m_screen, NULL, m_bgColor);
    SDL_UpdateRect(m_screen, 0, 0, 0, 0);
}

void FbSdl::fillRect(Rect* _r)
{
    SDL_Rect* r = TO_SDL_RECT(_r);

    SDL_FillRect(m_screen, r, m_fgColor);
}

inline void FbSdl::pset(int x, int y)
{
    *(((unsigned char*)m_screen->pixels) + y*m_screen->pitch + x) = m_fgColor;
}

void FbSdl::hline(int x1, int y, int x2)
{
    // TODO
    line(x1, y, x2, y);
}

void FbSdl::vline(int x, int y1, int y2)
{
    // TODO
    line(x, y1, x, y2);
}

void FbSdl::blit(unsigned char *p, int x, int y, int w, int h)
{
    clc::Log::trace("ocher.sdl", "blit %d %d %d %d", x, y, w, h);

    const int fbw = width();
    if (x + w >= fbw) {
        if (x >= fbw)
            return;
        w = fbw - x;
    }
    if (x < 0) {
        // TODO
    }
    const int fbh = height();
    if (y + h >= fbh) {
        if (y >= fbh)
            return;
        h = fbh - y;
    }
    if (y < 0) {
        // TODO
    }

    if (m_mustLock) {
        if (SDL_LockSurface(m_screen) < 0) {
            return;
        }
    }
    clc::Log::trace("ocher.sdl", "blit clipped %d %d %d %d", x, y, w, h);
    for (int i = 0; i < h; ++i) {
        unsigned char* dst = ((unsigned char*)m_screen->pixels) + y*m_screen->pitch + x;
        for (int ix = 0; ix < w; ++ix) {
            p[ix] = ~p[ix];
        }
        memcpy(dst, p, w);
        y++;
        p = p + w;
    }
    if (m_mustLock) {
        SDL_UnlockSurface(m_screen);
    }
}

int FbSdl::update(Rect* r, bool /*full*/)
{
    clc::Log::debug("ocher.sdl", "update");
    SDL_UpdateRects(m_screen, 1, (SDL_Rect*)r);
    return 0;
}

