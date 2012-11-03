#ifndef OCHER_UX_FONT_ENGINE_H
#define OCHER_UX_FONT_ENGINE_H


/**
 * Similar to memcpy
 * @param dst  Alpha bitmap
 * @param color  Color to apply to alpha of dst + src
 */
void *alphamemcpy(uint8_t *dst, const uint8_t *src, size_t n, uint8_t color);


/**
 * Lookup key for a glyph bitmap
 */
struct GlyphIndex
{
    uint32_t c;
    uint8_t faceId;
    uint8_t pts;
};

class Glyph
{
public:
    uint8_t* bitmap;
    uint8_t x;
    uint8_t y;
    int8_t dx;
    int8_t dy;
};

class GlyphCache
{
public:
    GlyphCache();
    virtual ~GlyphCache();

    put(GlyphIndex* i, );
    Glyph* get();

    clc::Hashtable m_cache;  ///< Maps GlyphIndex -> Glyph
};

/**
 * Converts glyphs to bitmaps.
 */
class FontEngine
{
public:
    FontEngine();
    virtual ~FontEngine();

    virtual bool init();

    void setSize(unsigned int points);

    virtual Glyph plotGlyph();

    /**
     * 
     */
    virtual void plotString(Glyph** glyphs, const char* str);

    GlyphCache m_cache;
};

#endif

