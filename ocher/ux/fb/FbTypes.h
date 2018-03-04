/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_TYPES_H
#define OCHER_UX_FB_TYPES_H

#include <cstdint>


struct Pos {
    int16_t x, y;

    Pos()
    {
    }

    Pos(int16_t _x, int16_t _y) :
        x(_x),
        y(_y)
    {
    }
};

struct Rect {
    int16_t x, y;
    uint16_t w, h;

    Rect()
    {
        setInvalid();
    }
    Rect(int16_t _x, int16_t _y, uint16_t _w, uint16_t _h) :
        x(_x),
        y(_y),
        w(_w),
        h(_h)
    {
    }
    const Pos *pos()
    {
        return reinterpret_cast<const Pos *>(this);
    }
    void offsetBy(Pos *p)
    {
        x += p->x;
        y += p->y;
    }
    void unionRect(Rect *r);
    void unionRects(Rect *r1, Rect *r2);
    bool contains(Pos *p)
    {
        return p->x >= x && p->y >= y && p->x < x + w && p->y < y + h;
    }
    bool valid()
    {
        return x + w >= 0 && y + h >= 0;
    }
    void setInvalid()
    {
        x = y = -1;
        w = h = 0;
    }
    void inset(int i)
    {
        x += i;
        y += i;
        w -= (i + i);
        h -= (i + i);
    }
};

class Glyph {
public:
    Glyph() :
        bitmap(0)
    {
    }
    ~Glyph()
    {
        delete[] bitmap;
    }
    uint8_t *bitmap;
    uint8_t w;
    uint8_t h;
    int8_t offsetX;
    int8_t offsetY;
    int8_t advanceX;
    int8_t advanceY;
    uint8_t height;
} __attribute__((packed));

struct GlyphDescr {
    GlyphDescr() : v(0) {}
    bool operator<(const GlyphDescr& r) const
    {
        return v < r.v;
    }

    union {
        struct {
            uint32_t c;
            uint8_t faceId;
            uint8_t points;
            int underline : 1;
            int bold : 1;
            int italic : 1;
        };
        uint64_t v;
    };
} __attribute__((packed));

#endif
