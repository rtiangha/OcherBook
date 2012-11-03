#ifndef OCHER_UX_FB_RLEIMAGE_H
#define OCHER_UX_FB_RLEIMAGE_H

#include <stdint.h>


/**
 * Used to store internal images.  Currently limited to 7bpp
 */
class RleBitmap
{
public:
    RleBitmap();

    void setTo(uint8_t* p, uint16_t w, uint16_t h);

    void copyFrom(uint8_t* p, uint16_t w, uint16_t h);

    void resetUnpack();

    /**
     */
    int unpackNext(uint8_t* c);

protected:
    bool m_del;
    uint8_t* m_p;
    uint16_t m_w;
    uint16_t m_h;
    unsigned int m_off;  ///< Offset of next byte to unpack
};

#endif

