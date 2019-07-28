/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/FontEngine.h"

#include "Container.h"
#include "settings/Settings.h"
#include "ux/fb/FrameBuffer.h"
#include "util/Logger.h"

#include <cctype>
#include <cstddef>

#define LOG_NAME "ocher.ux.FontEngine"


GlyphCache::~GlyphCache()
{
    for (auto it = m_cache.begin(); it != m_cache.end(); ) {
        Glyph::destroy(it->second);
        it = m_cache.erase(it);
    }
}

void GlyphCache::put(GlyphDescr& f, Glyph* g)
{
    auto search = m_cache.find(f);
    if (search != m_cache.end()) {
        Glyph::destroy(search->second);
    }
    m_cache[f] = g;
}

Glyph* GlyphCache::get(GlyphDescr& f)
{
    auto search = m_cache.find(f);
    if (search != m_cache.end()) {
        return search->second;
    } else {
        return nullptr;
    }
}

FontEngine::FontEngine(FrameBuffer* fb) :
    m_fb(fb)
{
    int r = FT_Init_FreeType(&m_lib);
    if (r) {
        Log::error(LOG_NAME, "FT_Init_FreeType failed: %d", r);
        throw std::runtime_error("FT_Init_FreeType failed"); // XXX
    }
}

FontEngine::~FontEngine()
{
    FT_Done_FreeType(m_lib);
}

FontContext FontEngine::context()
{
    FontFace face;
    face.points = g_container->settings.systemFontPoints;
    return context(face);
}

FontContext FontEngine::context(const FontFace& face)
{
    FontContext fc;
    return std::move(fc.setDPI(m_fb->ppi()).apply(*this, face));
}

void FontEngine::scanForFonts()
{
    const char* search;

    if (g_container->settings.fontRoot.length() == 0)
        search = ".";
    else
        search = g_container->settings.fontRoot.c_str();

    const char* p = search;
    const char* colon;
    colon = strchr(p, ':');
    if (colon) {
        // TODO
    }
}

static int utf8ToUtf32(const char* _p, uint32_t* u32)
{
    auto p = reinterpret_cast<const unsigned char*>(_p);
    int len = 1;
    uint32_t c = *p;

    if (c >= 0x7f) {
        if ((c & 0xe0) == 0xc0) {
            c = ((c & 0x1f) << 6) | (p[1] & 0x3f);
            len++;
        } else if ((c & 0xf0) == 0xe0) {
            c = ((c & 0x0f) << 12) | ((p[1] & 0x3f) << 6) | (p[2] & 0x3f);
            len += 2;
        } else if ((c & 0xf8) == 0xf0) {
            c = ((c & 0x07) << 18) | ((p[1] & 0x3f) << 12) | ((p[2] & 0x3f) << 6) | (p[3] & 0x3f);
            len += 3;
        } else if ((c & 0xfc) == 0xf8) {
            c = ((c & 0x03) << 24) | ((p[1] & 0x3f) << 18) | ((p[2] & 0x3f) << 12) | ((p[3] & 0x3f) << 6) | (p[4] & 0x3f);
            len += 4;
        } else if ((c & 0xfe) == 0xfc) {
            c = ((c & 0x01) << 30) | ((p[1] & 0x3f) << 24) | ((p[2] & 0x3f) << 18) | ((p[3] & 0x3f) << 12) | ((p[4] & 0x3f) << 6) | (p[5] & 0x3f);
            len += 5;
        } else {
            // out of sync?
            c = 0;
        }
    }
    *u32 = c;
    return len;
}

std::vector<Glyph*> FontEngine::calculateGlyphs(const FontContext& fc, const char* p,
        unsigned int len, Rect* bbox)
{
    GlyphDescr d;
    std::vector<Glyph*> glyphs;

    d.faceId    = fc.faceId();
    d.points    = fc.points();
    d.underline = fc.underline();
    d.bold      = fc.bold();
    d.italic    = fc.italic();

    bbox->w = 0;
    for (const char* end = p + len; p < end; ) {
        p += utf8ToUtf32(p, &d.c);

        Glyph* g = m_cache.get(d);
        if (!g) {
            if (!(g = fc.plotGlyph(&d))) {
                Log::warn(LOG_NAME, "plotGlyph failed for %x; skipping", d.c);
                continue;
            }

            invert(g->bitmap, g->w * g->h);
            m_cache.put(d, g);
        }

        glyphs.push_back(g);
        bbox->w += g->advanceX;
    }
    bbox->h = fc.lineHeight();
    return glyphs;
}

void FontEngine::blitGlyphs(const std::vector<Glyph*>& glyphs, Pos* pen, const Rect* clip)
{
    for (auto g : glyphs) {
        m_fb->blit(g->bitmap, pen->x + g->offsetX, pen->y - g->offsetY, g->w, g->h, clip);
        pen->x += g->advanceX;
        pen->y += g->advanceY;
    }
}

Rect FontEngine::blitString(const FontContext& fc, const char* str, unsigned int len, Pos* pen,
        const Rect* clip)
{
    Rect bbox;
    auto glyphs = calculateGlyphs(fc, str, len, &bbox);
    blitGlyphs(glyphs, pen, clip);
    return bbox;
}

// TODO:  return array of arrays of glyphs, and starting pos for each array
// or callback:  str, offset, len, bbox
unsigned int FontEngine::renderString(const FontContext& fc, const char* str, unsigned int len,
        Pos* pen, const Rect* r, unsigned int flags, Rect* bbox)
{
    const char* p = str;
    bool wordWrapped = false;

    if (bbox)
        bbox->clear();

    if (!(flags & FE_YCLIP) && pen->y >= r->h - fc.descender()) {
        return 0;
    }

    do {
        // If at start of line, eat spaces
        if (pen->x == 0) {
            while (len && isspace(*p)) {
                ++p;
                --len;
            }
        }

        if (*p != '\n') {
            // Where is the next word break?
            unsigned int w = 0;
            while (w < len && !isspace(*(p + w))) {
                ++w;
            }
            if (w < len)
                ++w;

            Rect wordBox;
            wordBox.x = pen->x;
            wordBox.y = pen->y;
            auto glyphs = calculateGlyphs(fc, p, w, &wordBox);
            if (flags & FE_WRAP &&           // want wrap
                pen->x + wordBox.w > r->w && // but wouldn't fit on this line
                wordBox.w <= r->w) {         // but would fit on next
                wordBox.x = pen->x = 0;
                pen->y += fc.lineHeight();
                wordBox.y = pen->y;
            }
            if (pen->y >= r->h - fc.descender())
                return p - str;
            wordBox.y -= fc.ascender();
            if (bbox)
                bbox->unionRect(wordBox);

            // Fits; render it and advance
            if (flags & FE_NOBLIT) {
                for (auto glyph : glyphs) {
                    pen->x += glyph->advanceX;
                    pen->y += glyph->advanceY;
                }
            } else {
                Pos dst;
                int xOffset = 0;
                if (flags & FE_XCENTER) {
                    for (auto glyph : glyphs) {
                        xOffset += glyph->advanceX;
                    }
                    xOffset = r->w / 2 - (xOffset >> 1);
                }
                dst.x = pen->x + r->x + xOffset;
                dst.y = pen->y + r->y;
                blitGlyphs(glyphs, &dst, r);
                pen->x = dst.x - r->x - xOffset;
                pen->y = dst.y - r->y;
            }
            p += w;
            len -= w;
        }

        // Word-wrap or hard linefeed, but avoid the two back-to-back.
        if (pen->x >= r->w && !(flags & FE_WRAP))
            return p - str;
        if ((*p == '\n' && !wordWrapped) || pen->x >= r->w) {
            pen->x = 0;
            pen->y += fc.lineHeight();
            if (*p == '\n' && len) {
                p++;
                len--;
            } else {
                wordWrapped = true;
            }
            if (pen->y >= r->h - fc.descender())
                return p - str;
        } else {
            if (*p == '\n') {
                p++;
                len--;
            }
            wordWrapped = false;
        }
    } while (len > 0);
    return -1;  // TODO
}
