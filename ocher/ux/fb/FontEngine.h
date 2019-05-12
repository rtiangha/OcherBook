/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FONT_ENGINE_H
#define OCHER_UX_FONT_ENGINE_H

#include "ux/fb/FbTypes.h"
#include "ux/fb/fontcontext.h"

#include <map>
#include <vector>

class FrameBuffer;

class GlyphCache {
public:
    ~GlyphCache();

    void put(GlyphDescr& f, Glyph* g);
    Glyph* get(GlyphDescr& f);

protected:
    std::map<GlyphDescr, Glyph*> m_cache;
};

/** Converts glyphs to bitmaps.
 */
class FontEngine {
public:
    FontEngine(FrameBuffer* fb);

    ~FontEngine();

    FontContext context();

    FontContext context(const FontFace& face);

    static void scanForFonts();

    /** Lays out the string in memory, calculating the glyphs and the bounding rectangle.
     */
    std::vector<Glyph*> calculateGlyphs(const FontContext& fc, const char* str, unsigned int len,
            Rect* boundingBox);

    /** Blits the glyphs to the framebuffer, moving the pen and optionally clipping.
     */
    void blitGlyphs(const std::vector<Glyph*>& glyphs, Pos* pen, const Rect* clip = nullptr);

    /**
     */
    Rect blitString(const FontContext& fc, const char* str, unsigned int len, Pos* pen,
            const Rect* clip = nullptr);


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
    unsigned int renderString(const FontContext& fc, const char* str, unsigned int len, Pos* pen,
            const Rect* r, unsigned int flags, Rect* bbox = nullptr);

    GlyphCache m_cache;

    FrameBuffer* m_fb = nullptr;
    FT_Library m_lib = nullptr;
};

#endif
