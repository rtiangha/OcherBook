/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef MX50_FB_H
#define MX50_FB_H

#include "ux/fb/FrameBuffer.h"

#include <linux/mxcfb.h>


class FrameBufferMx50 : public FrameBuffer {
public:
    FrameBufferMx50();
    ~FrameBufferMx50();

    bool init();

    unsigned int height() override;
    unsigned int width() override;
    unsigned int dpi() override;

    void setFg(uint8_t r, uint8_t b, uint8_t g) override;
    void setBg(uint8_t r, uint8_t b, uint8_t g) override;
    void clear() override;
    void fillRect(const Rect* r) override;
    void byLine(const Rect* r, void (*fn)(void* p, size_t n)) override;
    void pset(int x, int y) override;
    void hline(int x1, int y, int x2) override;
    inline void vline(int x, int y1, int y2) override
    {
        line(x, y1, x, y2);
    }
    void blit(const unsigned char* p, int x, int y, int w, int h, const Rect* clip) override;
    int update(const Rect* r, bool full = false) override;
    void sync() override;
    void needFull() override
    {
        m_needFull = true;
    }

    /**
     * @param marker  Waits on the specified update, or -1 for all
     */
    void waitUpdate(int marker = -1);

    void setPixelFormat();
    void setUpdateScheme();
    void setAutoUpdateMode(bool autoUpdate);

protected:
    int m_fd;
    char* m_fb;
    size_t m_fbSize;
    unsigned int m_marker;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    uint8_t m_fgColor;
    uint8_t m_bgColor;
    bool m_needFull;
};

#endif
