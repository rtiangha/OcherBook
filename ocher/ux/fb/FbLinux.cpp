/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/FbLinux.h"

#include "util/Logger.h"

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define LOG_NAME "ocher.fb"

FbLinux::~FbLinux()
{
    if (m_fd != -1) {
        close(m_fd);
        m_fd = -1;
    }
}

bool FbLinux::init()
{
    const char* dev = "/dev/fb0";

    m_fd = open(dev, O_RDWR);
    if (m_fd == -1) {
        Log::error(LOG_NAME, "Failed to open %s: %s", dev, strerror(errno));
        goto fail;
    }

    // Get fixed screen information
    if (ioctl(m_fd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        Log::error(LOG_NAME, "Failed to get fixed screen info: %s", strerror(errno));
        goto fail1;
    }
    char id[sizeof(finfo.id) + 1];
    memcpy(id, finfo.id, sizeof(finfo.id));
    id[sizeof(finfo.id)] = 0;
    Log::info(LOG_NAME, "id: %s", id);

    // Get variable screen information
    if (ioctl(m_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        Log::error(LOG_NAME, "Failed to get variable screen info: %s", strerror(errno));
        goto fail1;
    }

    initVarInfo();

    if (ioctl(m_fd, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
        Log::error(LOG_NAME, "Failed to set variable screen info: %s", strerror(errno));
        goto fail1;
    }

    // Figure out the size of the screen in bytes
    m_fbSize = vinfo.xres_virtual * vinfo.yres_virtual * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    m_fb = (char*)mmap(nullptr, m_fbSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
    if (m_fb == MAP_FAILED) {
        Log::error(LOG_NAME, "mmap: %s", strerror(errno));
        goto fail1;
    }

    setBg(0xff, 0xff, 0xff);
    clear();

    bbox.x = bbox.y = 0;
    bbox.w = xres();
    bbox.h = yres();
    Log::info(LOG_NAME, "res %dx%d virtual %dx%d offset %dx%d size %dx%dmm %u dpi",
            vinfo.xres, vinfo.yres,
            vinfo.xres_virtual, vinfo.yres_virtual,
            vinfo.xoffset, vinfo.yoffset,
            vinfo.width, vinfo.height,
            dpi());

    return true;

fail1:
    close(m_fd);
    m_fd = -1;
fail:
    return false;
}

unsigned int FbLinux::yres()
{
    return vinfo.yres;
}

unsigned int FbLinux::xres()
{
    return vinfo.xres;
}

unsigned int FbLinux::dpi()
{
    return vinfo.xres / (vinfo.width / 25.4);
}

static inline uint8_t getColor(uint8_t r, uint8_t b, uint8_t g)
{
    return (((uint16_t)r * 61) + ((uint16_t)g * 174) + ((uint16_t)b * 21)) >> 8;
}

void FbLinux::setFg(uint8_t r, uint8_t b, uint8_t g)
{
    m_fgColor = getColor(r, b, g);
}

void FbLinux::setBg(uint8_t r, uint8_t b, uint8_t g)
{
    m_bgColor = getColor(r, b, g);
}

void FbLinux::clear()
{
    memset(m_fb, m_bgColor, m_fbSize);
}

void FbLinux::pset(int x, int y)
{
    *(((unsigned char*)m_fb) + y * vinfo.xres_virtual + x) = m_fgColor;
}

void FbLinux::hline(int x1, int y, int x2)
{
    int dx = x2 - x1;

    if (dx < 0)
        dx = -dx;
    memset(m_fb + y * vinfo.xres_virtual + x1, m_fgColor, dx + 1);
}

void FbLinux::fillRect(const Rect* r)
{
    int y2 = r->y + r->h;

    for (int y = r->y; y < y2; ++y) {
        hline(r->x, y, r->x + r->w - 1);
    }
}

void FbLinux::byLine(const Rect* r, void (*fn)(void* p, size_t n))
{
    int y2 = r->y + r->h;

    for (int y = r->y; y < y2; ++y) {
        fn(((unsigned char*)m_fb) + y * vinfo.xres_virtual + r->x, r->w);
    }
}

void FbLinux::blit(const unsigned char* p, int x, int y, int w, int h, const Rect* userClip)
{
    Log::trace(LOG_NAME, "blit %d %d %d %d", x, y, w, h);
    int rectWidth = w;
    Rect clip;

    if (userClip) {
        clip = *userClip;
    } else {
        clip.x = clip.y = 0;
        clip.w = xres();
        clip.h = yres();
    }

    const int maxX = clip.x + clip.w - 1;
    const int minX = clip.x;
    if (x + w >= maxX) {
        if (x >= maxX)
            return;
        w = maxX - x;
    }
    if (x < minX) {
        // TODO
    }
    const int maxY = clip.y + clip.h - 1;
    const int minY = clip.y;
    if (y + h >= maxY) {
        if (y >= maxY)
            return;
        h = maxY - y;
    }
    if (y < minY) {
        // TODO
    }

    Log::trace(LOG_NAME, "blit clipped %d %d %d %d", x, y, w, h);
    unsigned char* dst = ((unsigned char*)m_fb) + y * vinfo.xres_virtual + x;
    for (int i = 0; i < h; ++i) {
        memAnd(dst, p, w);
        dst += vinfo.xres_virtual;
        p += rectWidth;
    }
}
