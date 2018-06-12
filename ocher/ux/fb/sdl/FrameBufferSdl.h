/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef SDL_FB_H
#define SDL_FB_H

#include "ux/fb/FrameBuffer.h"
#include "ux/fb/sdl/SdlThread.h"

#include <SDL.h>


class FrameBufferSdl : public FrameBuffer {
public:
    FrameBufferSdl();
    virtual ~FrameBufferSdl();

    bool init() override;

    unsigned int height() override;
    unsigned int width() override;
    unsigned int dpi() override;

    void inject(EventLoop* loop) override;

    void setFg(uint8_t r, uint8_t b, uint8_t g) override;
    void setBg(uint8_t r, uint8_t b, uint8_t g) override;
    void clear() override;
    void fillRect(Rect* r) override;
    void byLine(Rect* r, void (*fn)(void* p, size_t n)) override;
    void pset(int x, int y) override;
    void hline(int x1, int y, int x2) override;
    void vline(int x, int y1, int y2) override;
    void blit(const unsigned char* p, int x, int y, int w, int h, const Rect* clip) override;
    int update(Rect* r, bool full = false) override;

protected:
    SdlThread m_sdlThread;

    int m_sdl;
    SDL_Surface* m_screen;
    bool m_mustLock;
    uint8_t m_fgColor;
    uint8_t m_bgColor;

    uint8_t getColor(uint8_t r, uint8_t b, uint8_t g);
};

#endif
