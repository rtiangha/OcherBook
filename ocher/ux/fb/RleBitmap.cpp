#include "ocher/ux/fb/RleBitmap.h"


RleBitmap::RleBitmap() :
    m_p(0)
{
}

void RleBitmap::compress(uint8_t* p, unsigned int len)
{
    uint8_t count = 0;
    uint8_t runOf = ~*p;  // only set when count != 0
    for (unsigned int i = 0; i < len; ++i) {
        if (p[i] == runOf) {
            while (count < 255 && i < len) {
                ++i;
                if (p[i] == runOf)

            }
        } else {
        }

        if (count) {
            if (p[i] == runOf && count < 255) {
                count++;
            } else {
                out(count);
                count = 0;
                --i;
            }
        } else if (p[i] == runOf) {
            out(p[i]);
            count++;
        } else {
            g
        }
    }

}

void RleBitmap::resetUnpack()
{
    m_off = 0;
}

int RleBitmap::unpackNext(uint8_t* c)
{
    
}
