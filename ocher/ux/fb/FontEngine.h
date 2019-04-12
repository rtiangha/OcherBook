/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FONT_ENGINE_H
#define OCHER_UX_FONT_ENGINE_H

#include "ux/fb/FbTypes.h"
#include "ux/fb/FreeType.h"

#include <map>

class FrameBuffer;

/**
 */
struct GlyphFace {
    uint8_t faceId;
    uint8_t points;
    uint8_t underline;
    uint8_t bold;
    uint8_t italic;

    int16_t bearingY;
    int16_t ascender;
    int16_t descender;
    int16_t lineHeight;
    int16_t underlinePos;
} __attribute__((packed));

class GlyphCache {
public:
    ~GlyphCache();

    void put(GlyphDescr& f, Glyph* g);
    Glyph* get(GlyphDescr& f);

protected:
    std::map<GlyphDescr, Glyph*> m_cache;
};

/** Converts glyphs to bitmaps.
 *
 * @todo  GlyphCache/FreeType should be shared; face is per local context
 */
class FontEngine {
public:
    FontEngine(FrameBuffer* fb);

    static void scanForFonts();  // TODO move to FreeType?

    void setFont();
    void setSize(unsigned int points);
    void setBold(int b);
    void setUnderline(int u);
    void setItalic(int i);
    void apply();
    int dirty;
    GlyphFace m_cur;
    GlyphFace m_next;

    void blitGlyphs(Glyph** glyphs, Pos* pen, const Rect* clip = nullptr);

    /**
     *
     */
    void plotString(const char* str, unsigned int len, Glyph** glyphs, Rect* boundingBox);
// TODO            int16_t bearingX, int16_t bearingY);

    /** Renders a string, in the current font.
     *
     * @param str  Data to render
     * @param len
     * @param pen  Position of starting baseline, relative to r.  Updated.
     * @param r  Render relative to containing rectangle (possibly clip rectangle)
     * @param flags
     *      NOBLIT   Don't blit to framebuffer, just measure
     *      WRAP     Word wrap?
     *      XCLIP    Render clipped char, or stop if char does not fit.  N/A if WRAP.
     *      YCLIP    Render clipped char, or stop if char does not fit.
     *      XCENTER  Centers horizontally within the rect.  Currently incompatible with WRAP.
     *               TODO buggy
     * @param bbox Bounding box containing all output
     * @return offset of first unrendered character (==len if all fit).  May also stop early if len
     *      splits a multi-byte character.
     */
#define FE_NOBLIT   1
#define FE_WRAP     2
#define FE_XCLIP    4
#define FE_YCLIP    8
#define FE_XCENTER 16
    unsigned int renderString(const char* str, unsigned int len, Pos* pen, const Rect* r, unsigned int flags,
            Rect* bbox = nullptr);

    GlyphCache m_cache;

    FrameBuffer* m_fb;
    FreeType m_ft;
};

#endif
