#ifndef SDL_FB_H
#define SDL_FB_H

#include "SDL/SDL.h"

#include "ocher/ux/fb/FrameBuffer.h"


class FbSdl : public FrameBuffer
{
public:
    FbSdl();
    virtual ~FbSdl();

    bool init();

    unsigned int height();
    unsigned int width();
    unsigned int dpi();

    void setFg(uint8_t r, uint8_t b, uint8_t g);
    void setBg(uint8_t r, uint8_t b, uint8_t g);
    void clear();
    void pset(int x, int y);
    void hline(int x1, int y, int x2);
    void vline(int x, int y1, int y2);
    void blit(unsigned char* p, int x, int y, int w, int h, const Rect* clip);
    void fillRect(Rect* r);
    int update(Rect* r, bool full=true);

protected:
    int m_sdl;
    SDL_Surface* m_screen;
    bool m_mustLock;
    uint8_t m_fgColor;
    uint8_t m_bgColor;

    uint8_t getColor(uint8_t r, uint8_t b, uint8_t g);
};

#endif
