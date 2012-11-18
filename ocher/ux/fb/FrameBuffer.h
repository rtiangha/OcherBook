#ifndef OCHER_FRAMEBUFFER_H
#define OCHER_FRAMEBUFFER_H

#include "ocher/ux/fb/FbTypes.h"


class FrameBuffer
{
public:
    FrameBuffer() {}
    virtual ~FrameBuffer() {}

    Rect bbox;

    virtual unsigned int height() = 0;
    virtual unsigned int width() = 0;
    virtual unsigned int dpi() = 0;

    virtual void setFg(uint8_t r, uint8_t b, uint8_t g) = 0;
    virtual void setBg(uint8_t r, uint8_t b, uint8_t g) = 0;
    virtual void clear() = 0;
    virtual void rect(Rect* rect);
    virtual void fillRect(Rect* r) = 0;
    virtual void roundRect(Rect* rect, unsigned int radius);

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
    virtual void hline(int x1, int y, int x2) { line(x1, y, x2, y); }

    /**
     * No clipping.
     */
    virtual void vline(int x, int y1, int y2) { line(x, y1, x, y2); }

    /**
     * Clips to the framebuffer and blits.
     * @param p  'w' x 'h' bitmap.  Matches the bpp of the framebuffer.
     */
    virtual void blit(unsigned char* p, int x, int y, int w, int h, const Rect* clip=0) = 0;

    /**
     */
    void blitGlyphs(Glyph** glyphs, Pos* pen, const Rect* clip=0);

    virtual int update(Rect* r, bool full=false) = 0;

    /**
     * Ensures that all prior updates have completed.
     */
    virtual void sync() {}
};

#endif

