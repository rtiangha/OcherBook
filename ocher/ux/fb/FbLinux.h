/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef LINUX_FB_H
#define LINUX_FB_H

#include "ux/fb/FrameBuffer.h"

#include <linux/fb.h>


class FbLinux : public FrameBuffer {
public:
    FbLinux() = default;
    ~FbLinux();

    bool init() override;
    virtual void initVarInfo() {}

    int yres() override;
    int xres() override;
    int ppi() override;

    void setFg(uint8_t r, uint8_t b, uint8_t g) override;
    void setBg(uint8_t r, uint8_t b, uint8_t g) override;
    void clear() override;
    void fillRect(const Rect* r) override;
    void byLine(const Rect* r, void (*fn)(void* p, size_t n)) override;
    void pset(int x, int y) override;
    void hline(int x1, int y, int x2) override;
    void blit(const unsigned char* p, int x, int y, int w, int h, const Rect* clip) override;

protected:
    int m_fd = -1;
    char* m_fb = nullptr;
    size_t m_fbSize = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    uint8_t m_fgColor;
    uint8_t m_bgColor;
};

#endif

