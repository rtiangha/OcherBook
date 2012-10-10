#include <ctype.h>

#include "ocher/device/Device.h"
#include "ocher/ux/fb/FreeType.h"
#include "ocher/ux/fb/FrameBuffer.h"

#include "clc/support/Logger.h"

// TODO: something is broken
#undef GLYPH_CACHE

FreeType::FreeType(FrameBuffer *fb) :
    m_lib(0),
    m_cache(0),
    m_size(12),
    m_fb(fb)
{
}

FreeType::~FreeType()
{
    if (m_cache)
        FTC_Manager_Done(m_cache);
    FT_Done_FreeType(m_lib);
}

FT_Error FreeType::faceRequester(FTC_FaceID faceID, FT_Library lib, FT_Pointer reqData, FT_Face *face)
{
    clc::Log::debug("ocher.freetype", "face request");
    int r = FT_New_Face(lib, "FreeSans.otf", 0, face);
    ASSERT(r == 0);(void)r;
    FreeType* self = (FreeType*)reqData;
    self->m_face = *face;
    return 0;
}

bool FreeType::init()
{
    int r;
    r = FT_Init_FreeType(&m_lib);
    if (r) {
        clc::Log::error("ocher.freetype", "FT_Init_FreeType failed: %d", r);
        return false;
    }

#ifndef GLYPH_CACHE
    r = FT_New_Face(m_lib, "FreeSans.otf", 0, &m_face);
    if (r || !m_face) {
        clc::Log::error("ocher.freetype", "FT_New_Face failed: %d", r);
        return false;
    }
#else
    r = FTC_Manager_New(m_lib, 10, 10, 10*1024*1024, faceRequester, this, &m_cache);
    r |= FTC_CMapCache_New(m_cache, &m_cmapCache);
    r |= FTC_SBitCache_New(m_cache, &m_sbitCache);
    if (r) {
        clc::Log::error("ocher.freetype", "FTC_Manager_New failed: %d", r);
    }
#endif
    return true;
}

void FreeType::setSize(unsigned int points)
{
#ifndef GLYPH_CACHE
    FT_Set_Char_Size(m_face, 0, points*64, m_fb->dpi(), m_fb->dpi());
#else
    m_size = points*64;
#endif
}

bool FreeType::renderString(const char* s, bool doBlit, int penX, int penY, int* dx, int* dy, uint8_t* height)
{
    while (1) {
        int c = *s;
        if (!c)
            break;
        int cdx, cdy;
        if (renderGlyph(c, doBlit, penX, penY, &cdx, &cdy, height)) {
            *dx += cdx;
            *dy += cdy;
            penX += cdx;
            penY += cdy;
        }
        s++;
    }
    return true;
}

bool FreeType::renderGlyph(int c, bool doBlit, int penX, int penY, int* dx, int* dy, uint8_t* height)
{
#ifndef GLYPH_CACHE
    unsigned int glyphIndex = FT_Get_Char_Index(m_face, c);
    int r = FT_Load_Glyph(m_face, glyphIndex, FT_LOAD_DEFAULT);
    if (r) {
        clc::Log::error("ocher.freetype", "FT_Load_Glyph failed: %d", r);
        return false;
    }
    if (m_face->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
        r = FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_NORMAL);
        if (r) {
            clc::Log::error("ocher.freetype", "FT_Render_Glyph failed: %d", r);
            return false;
        }
    }

    if (doBlit) {
        m_fb->blit(m_face->glyph->bitmap.buffer, penX + m_face->glyph->bitmap_left,
                penY - m_face->glyph->bitmap_top, m_face->glyph->bitmap.width,
                m_face->glyph->bitmap.rows);
    }

    FT_GlyphSlot slot = m_face->glyph;
    *dx = slot->advance.x >> 6;
    *dy = slot->advance.y >> 6;
    *height = m_face->size->metrics.height >> 6;
#else
    unsigned int glyphIndex = FTC_CMapCache_Lookup(m_cmapCache, 0/*TODO faceId*/, -1, c);

    FTC_ImageTypeRec ftcImageType;
    ftcImageType.face_id = 0;   // TODO faceId
    ftcImageType.width = m_size;
    ftcImageType.height = m_size;
    ftcImageType.flags = FT_LOAD_DEFAULT | FT_LOAD_RENDER;

    FTC_SBit sbit;
    FTC_Node ftcNode;
    int r = FTC_SBitCache_Lookup(m_sbitCache, &ftcImageType, glyphIndex, &sbit, &ftcNode);

    if (doBlit) {
        m_fb->blit(sbit->buffer, penX + sbit->left, penY - sbit->top, sbit->width, sbit->height);
    }

    *dx = sbit->xadvance;
    *dy = sbit->yadvance;
    *height = m_face->size->metrics.height >> 6;

    FTC_Node_Unref(ftcNode, m_cache);
#endif

    return true;
}

