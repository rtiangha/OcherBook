/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_FRAMEBUFFER_H
#define OCHER_FRAMEBUFFER_H

#include "ux/Event.h"
#include "ux/fb/FbTypes.h"

#include <cstddef>


void invert(void* p, size_t n);
void dim(void* p, size_t n);
void fade(void* p, size_t n);
inline void memAnd(void* dst, const void* src, size_t n)
{
    auto d = (uint8_t*)dst;
    auto s = (const uint8_t*)src;

    while (n--) {
        *d++ &= *s++;
    }
}
inline void memOr(void* dst, const void* src, size_t n)
{
    auto d = (uint8_t*)dst;
    auto s = (const uint8_t*)src;

    while (n--) {
        *d++ |= *s++;
    }
}

class FrameBuffer {
public:
    FrameBuffer() = default;
    virtual ~FrameBuffer() = default;

    Rect bbox;

    virtual bool init() = 0;

    virtual int yres() = 0;
    virtual int xres() = 0;
    virtual int ppi() = 0;

    virtual void setFg(uint8_t r, uint8_t b, uint8_t g) = 0;
    virtual void setBg(uint8_t r, uint8_t b, uint8_t g) = 0;
    virtual void clear() = 0;
    virtual void rect(const Rect* rect);
    virtual void fillRect(const Rect* r) = 0;
    virtual void byLine(const Rect* r, void (*fn)(void* p, size_t n)) = 0;
    virtual void roundRect(const Rect* rect, unsigned int radius);

    /**
     * Sets a pixel in the current color.
     * No clipping.
     */
    virtual void pset(int x, int y) = 0;

    /**
     * Default implementation
     * No clipping.
     */
    virtual void line(int x1, int y1, int x2, int y2);

    /**
     * No clipping.
     */
    virtual void hline(int x1, int y, int x2)
    {
        line(x1, y, x2, y);
    }

    /**
     * No clipping.
     */
    virtual void vline(int x, int y1, int y2)
    {
        line(x, y1, x, y2);
    }

    /**
     * Clips to the framebuffer and blits.
     * @param p  'w' x 'h' bitmap.  Matches the bpp of the framebuffer.
     * @param x
     * @param y
     * @param w
     * @param h
     * @param clip
     */
    virtual void blit(const unsigned char* p, int x, int y, int w, int h, const Rect* clip = nullptr) = 0;

    virtual int update(const Rect* r, bool full = false)
    {
        return 0;
    }

    /**
     * Ensures that all prior updates have completed.
     */
    virtual void sync()
    {
    }

    virtual void needFull()
    {
    }
};

#endif
