/*
 * Copyright (c) 2019, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_FONTCONTEXT_H
#define OCHER_FONTCONTEXT_H

#include "FbTypes.h"

#include <ft2build.h>
#include FT_CACHE_H
#include FT_FREETYPE_H

class FontEngine;

/**
 */
struct FontFace {
    uint8_t faceId = 0; // TODO
    int points = 12;
    bool underline = false;
    bool bold = false;
    bool italic = false;
};

class FontContext {
public:
    FontContext(unsigned int dpi);

    FontContext& setPoints(int points);

    FontContext& apply(FontEngine& engine, const FontFace& face);

    inline uint8_t faceId() const
    {
        return m_cur.faceId;
    }

    inline int points() const
    {
        return m_cur.points;
    }

    inline bool bold() const
    {
        return m_cur.bold;
    }

    inline bool italic() const
    {
        return m_cur.italic;
    }

    inline bool underline() const
    {
        return m_cur.underline;
    }

    inline int ascender() const
    {
        return m_ascender;
    }

    inline int descender() const
    {
        return m_descender;
    }

    inline int bearingY() const
    {
        return m_bearingY;
    }

    inline int lineHeight() const
    {
        return m_lineHeight;
    }

    inline int underlinePos() const
    {
        return m_underlinePos;
    }

    Glyph* plotGlyph(const GlyphDescr* f) const;

protected:
    bool setFace(FontEngine& engine, const FontFace& face);

    unsigned int m_dpi;

    FT_Face m_face = nullptr;

    FontFace m_cur;

    int m_ascender;
    int m_descender;
    int m_bearingY;
    int m_lineHeight;
    int m_underlinePos;
};

#endif
