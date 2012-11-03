#include "ocher/ux/fb/FontEngine.h"


FontEngine::FontEngine()
{
}

FontEngine::~FontEngine()
{
}


int utf8toUtf32(unsigned char* p, uint32_t* u32)
{
    int len;
    uint32_t c = *p;
    if (c > 0x7f) {
        // Convert UTF8 to UTF32, as required by FreeType
        if ((c & 0xe0) == 0xc0) {
            c = ((c & 0x1f) << 6) | (p[1] & 0x3f);
            len = 1;
        } else if ((c & 0xf0) == 0xe0) {
            c = ((c & 0x0f) << 12) | ((p[1] & 0x3f) << 6) | (p[2] & 0x3f);
            len = 2;
        } else if ((c & 0xf8) == 0xf0) {
            c = ((c & 0x07) << 18) | ((p[1] & 0x3f) << 12) | ((p[2] & 0x3f) << 6) | (p[3] & 0x3f);
            len = 3;
        } else if ((c & 0xfc) == 0xf8) {
            c = ((c & 0x03) << 24) | ((p[1] & 0x3f) << 18) | ((p[2] & 0x3f) << 12) | ((p[3] & 0x3f) << 6) | (p[4] & 0x3f);
            len = 4;
        } else if ((c & 0xfe) == 0xfc) {
            c = ((c & 0x01) << 30) | ((p[1] & 0x3f) << 24) | ((p[2] & 0x3f) << 18) | ((p[3] & 0x3f) << 12) | ((p[4] & 0x3f) << 6) | (p[5] & 0x3f);
            len = 5;
        } else {
            // out of sync?
            continue;
        }
    }
    return len;
}

void FontEnginer::plotString(Glyph** glyphs, const char* str)
{
    GlyphIndex i;
    for (const char* p = str; *p; ++p) {
        uint32_t u32 = utf8ToUtf32(p);

        Glyph* glyph = m_cache.get(u32, );

        if (!glyph) {

            m_cache.put(
        }


    }

}
