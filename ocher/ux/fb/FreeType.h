#ifndef OCHER_FREETYPE_H
#define OCHER_FREETYPE_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H

class FrameBuffer;

class FreeType
{
public:
    FreeType(FrameBuffer* fb);
    ~FreeType();

    bool init();

    void setSize(unsigned int points);
    bool renderGlyph(int c, bool doBlit, int penX, int penY, int* dx, int* dy, uint8_t* height);
    bool renderString(const char* s, bool doBlit, int penX, int penY, int* dx, int* dy, uint8_t* height);

    //int getAscender() { return m_face->ascender; }
    int getAscender() { return m_face->size->metrics.ascender / 64; }

protected:
    static FT_Error faceRequester(FTC_FaceID faceID, FT_Library lib, FT_Pointer reqData, FT_Face* face);

    FT_Library m_lib;
    FT_Face m_face;
    FTC_Manager m_cache;
    FTC_CMapCache m_cmapCache;
    FTC_SBitCache m_sbitCache;
    int m_size;

    FrameBuffer *m_fb;
};

#endif

