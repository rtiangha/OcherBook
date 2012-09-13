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
    bool renderGlyph(int c, bool doBlit, int penX, int penY, int* dx, int* dy, int* height);

protected:
    static FT_Error faceRequester(FTC_FaceID faceID, FT_Library lib, FT_Pointer reqData, FT_Face* face);

    FT_Library m_lib;
    FT_Face m_face;
    FTC_Manager m_cache;
    FTC_CMapCache m_cmapCache;
    FTC_SBitCache m_sbitCache;

    FrameBuffer *m_fb;
};

#endif

