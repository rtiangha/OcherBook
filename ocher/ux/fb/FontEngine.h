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

};
/**
 * Converts glyphs to bitmaps.
 */
class FontEngine
{
public:
    ~FontEngine();

    virtual bool init();

    void setSize(unsigned int points);

    virtual void plotString(Glyph** glyphs, const char* str);

protected:
    clc::Hashtable m_cache;  ///< Maps GlyphIndex -> Glyph
    struct GlyphIndex i;
};

#endif

