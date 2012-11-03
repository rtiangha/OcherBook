#include "ocher/ux/fb/RleBitmap.h"


RleBitmap::RleBitmap() :
    m_p(0)
{
}

void RleBitmap::setTo(uint8_t* p, uint16_t w, uint16_t h)
{
    m_del = false;
    m_p = p;
    m_w = w;
    m_h = h;
}

void RleBitmap::resetUnpack()
{
    m_off = 0;
}

int RleBitmap::unpackNext(uint8_t* c)
{
    
}


