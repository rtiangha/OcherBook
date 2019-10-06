/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher_config.h"

#include "ux/fb/FontEngine.h"
#include "ux/fb/fontcontext.h"

#include "Container.h"
#include "settings/Settings.h"
#include "util/Logger.h"

#include <cctype>

#define LOG_NAME "ocher.freetype"



static const char* ttfFiles[] = {
    // TODO
#ifdef OCHER_TARGET_KOBO
    "Delima.ttf",
    "Delima-Italic.ttf",
    "Delima-Bold.ttf",
    "Delima-BoldItalic.ttf",
#else
#if 0
    "liberation/LiberationSerif-Regular.ttf",
    "liberation/LiberationSerif-Italic.ttf",
    "liberation/LiberationSerif-Bold.ttf",
    "liberation/LiberationSerif-BoldItalic.ttf",
#else
    "DejaVuSerif.ttf",
    "DejaVuSerif-Italic.ttf",
    "DejaVuSerif-Bold.ttf",
    "DejaVuSerif-BoldItalic.ttf",
#endif
#endif
};

FontContext::FontContext(FontContext&& other)
{
    *this = std::move(other);
}

FontContext& FontContext::operator=(FontContext&& other)
{
    m_dpi          = other.m_dpi;
    m_face         = other.m_face;
    other.m_face   = nullptr;
    m_cur          = other.m_cur;
    m_ascender     = other.m_ascender;
    m_descender    = other.m_descender;
    m_bearingY     = other.m_bearingY;
    m_lineHeight   = other.m_lineHeight;
    m_underlinePos = other.m_underlinePos;
    return *this;
}

FontContext::~FontContext()
{
    doneFace();
}

void FontContext::doneFace()
{
    if (m_face != nullptr) {
        FT_Done_Face(m_face);
        m_face = nullptr;
    }
}

bool FontContext::setFace(FontEngine& engine, const FontFace& face)
{
    std::string file = g_container->settings.fontRoot;

    int i = face.italic ? 1 : 0;
    int b = face.bold ? 1 : 0;
    file += "/";
    file += ttfFiles[i + b * 2];

    doneFace();
    int r = FT_New_Face(engine.m_lib, file.c_str(), 0, &m_face);
    if (r || !m_face) {
        Log::error(LOG_NAME, "FT_New_Face(\"%s\") failed: %d", file.c_str(), r);
        return false;
    }
    return true;
}

FontContext& FontContext::setPoints(int points)
{
    if (m_face != nullptr) {
        FT_Set_Char_Size(m_face, 0, points * 64, m_dpi, m_dpi);
        m_cur.points = points;

        m_ascender   = m_face->size->metrics.ascender >> 6;
        m_descender  = m_face->size->metrics.descender >> 6;
        m_bearingY   = m_face->size->metrics.ascender >> 6; // TODO
        m_lineHeight = m_face->size->metrics.height >> 6;
        m_underlinePos = (-m_face->underline_position) >> 6;
    }

    return *this;
}

FontContext& FontContext::apply(FontEngine& engine, const FontFace& face)
{
    setFace(engine, face);
    setPoints(face.points);
    m_cur = face;
    return *this;
}

Glyph* FontContext::plotGlyph(const GlyphDescr* d) const
{
    if (m_face == nullptr)
        return nullptr;

    FT_GlyphSlot slot = m_face->glyph;
    int r = FT_Load_Char(m_face, d->c, FT_LOAD_RENDER);
    if (r) {
        Log::error(LOG_NAME, "FT_Load_Char failed: %d", r);
        return nullptr;
    }

    size_t bytes = slot->bitmap.width * slot->bitmap.rows;
    Glyph* g = Glyph::create(bytes);
    if (g) {
        g->w = slot->bitmap.width;
        g->h = slot->bitmap.rows;
        memcpy(g->bitmap, slot->bitmap.buffer, bytes);
        g->offsetX = slot->bitmap_left;
        g->offsetY = slot->bitmap_top;
        g->advanceX = slot->advance.x >> 6;
        g->advanceY = slot->advance.y >> 6;
        g->height = m_face->size->metrics.height >> 6;
    }
    return g;
}
