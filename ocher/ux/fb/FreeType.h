/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_FREETYPE_H
#define OCHER_FREETYPE_H

#include "FbTypes.h"

#include "ft2build.h"
#include FT_CACHE_H
#include FT_FREETYPE_H


class FreeType {
public:
    FreeType(unsigned int dpi);
    ~FreeType();

    bool init();

    bool setFace(int i, int b);
    void setSize(unsigned int points);
    int getDescender()
    {
        return m_face->size->metrics.descender >> 6;
    }
    int getAscender()
    {
        return m_face->size->metrics.ascender >> 6;
    }
    int getLineHeight()
    {
        return m_face->size->metrics.height >> 6;
    }
    int getUnderlinePos()
    {
        return (-m_face->underline_position) >> 6;
    }

    int plotGlyph(GlyphDescr *f, Glyph *g);

protected:
    FT_Face m_face;
    FT_Library m_lib;
    unsigned int m_dpi;
};

#endif
