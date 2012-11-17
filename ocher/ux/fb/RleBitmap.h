#ifndef OCHER_UX_FB_RLEIMAGE_H
#define OCHER_UX_FB_RLEIMAGE_H

#include <stdint.h>


/**
 * Used to store simple images, such as line drawings.
 * Two consecutive identical bytes indicate a run; the next byte indicates how
 * many additional such bytes follow (0-255).
 */
class RleBitmap
{
public:
    RleBitmap();

    /**
     * Imports the uncompressed buffer.
     */
    void compress(uint8_t* p, unsigned int len);


    void resetUnpack();

    /**
     * Unpacks the next
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
