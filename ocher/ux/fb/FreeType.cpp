/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher_config.h"

#include "ux/fb/FreeType.h"

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

FreeType::FreeType(unsigned int dpi) :
    m_face(nullptr),
    m_lib(nullptr),
    m_dpi(dpi)
{
    int r = FT_Init_FreeType(&m_lib);
    if (r) {
        Log::error(LOG_NAME, "FT_Init_FreeType failed: %d", r);
        throw std::runtime_error("FT_Init_FreeType failed"); // XXX
    }
    setFace(0, 0);  // XXX internal error state on failure
}

FreeType::~FreeType()
{
    FT_Done_FreeType(m_lib);
}

bool FreeType::setFace(int i, int b)
{
    std::string file = g_container->settings.fontRoot;

    i = i ? 1 : 0;
    b = b ? 1 : 0;
    file += "/";
    file += ttfFiles[i + b * 2];

    int r = FT_New_Face(m_lib, file.c_str(), 0, &m_face);
    if (r || !m_face) {
        Log::error(LOG_NAME, "FT_New_Face(\"%s\") failed: %d", file.c_str(), r);
        return false;
    }
    return true;
}

void FreeType::setSize(unsigned int points)
{
    FT_Set_Char_Size(m_face, 0, points * 64, m_dpi, m_dpi);
}

Glyph* FreeType::plotGlyph(GlyphDescr* d)
{
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
