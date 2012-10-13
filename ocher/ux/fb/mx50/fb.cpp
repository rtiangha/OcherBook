#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define uint unsigned int
#include <linux/mxcfb.h>

#include "clc/support/Logger.h"

#include "ocher/ux/fb/mx50/fb.h"
#include "ocher/settings/Settings.h"


// Kobo Touch: MX508
// http://mediaz.googlecode.com/svn-history/r19/trunk/ReaderZ/native/einkfb/einkfb.c

Mx50Fb::Mx50Fb() :
    m_fd(-1),
    m_fb(0),
    m_fbSize(0),
    m_marker(-1),
    m_clears(settings.fullRefreshPages)
{
}

bool Mx50Fb::init()
{
    const char *dev = "/dev/fb0";
    m_fd = open(dev, O_RDWR);
    if (m_fd == -1) {
        clc::Log::error("ocher.mx50", "Failed to open %s: %s", dev, strerror(errno));
        goto fail;
    }

    // Get fixed screen information
    if (ioctl(m_fd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        clc::Log::error("ocher.mx50", "Failed to get fixed screen info: %s", strerror(errno));
        goto fail1;
    }

    // Get variable screen information
    if (ioctl(m_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        clc::Log::error("ocher.mx50", "Failed to get variable screen info: %s", strerror(errno));
        goto fail1;
    }

    // Configure for what we actually want
    vinfo.bits_per_pixel = 8;
    vinfo.grayscale = 1;
    // 0 is landscape right handed, 3 is portrait
    vinfo.rotate = 3;
    if (ioctl(m_fd, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
        clc::Log::error("ocher.mx50", "Failed to set variable screen info: %s", strerror(errno));
        goto fail1;
    }

    clc::Log::info("ocher.mx50", "virtual %dx%d", vinfo.xres_virtual, vinfo.yres_virtual);
    // Figure out the size of the screen in bytes
    m_fbSize = vinfo.xres_virtual * vinfo.yres_virtual * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    m_fb = (char*)mmap(0, m_fbSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
    if (m_fb == MAP_FAILED) {
        clc::Log::error("ocher.mx50", "mmap: %s", strerror(errno));
        goto fail1;
    }

    setBg(0xff, 0xff, 0xff);
    clear();
    update(NULL, true);
    clc::Log::info("ocher.mx50", "initialized");
    return true;

fail1:
    close(m_fd);
    m_fd = -1;
fail:
    return false;
}

unsigned int Mx50Fb::height()
{
    return 800;
}

unsigned int Mx50Fb::width()
{
    return 600;
}

Mx50Fb::~Mx50Fb()
{
    if (m_fd != -1) {
        close(m_fd);
        m_fd = -1;
    }
}

static inline uint8_t getColor(uint8_t r, uint8_t b, uint8_t g)
{
    return (((uint16_t)r*61)+((uint16_t)g*174)+((uint16_t)b*21))>>8;
}

void Mx50Fb::setFg(uint8_t r, uint8_t b, uint8_t g)
{
    m_fgColor = getColor(r, b, g);
}

void Mx50Fb::setBg(uint8_t r, uint8_t b, uint8_t g)
{
    m_bgColor = getColor(r, b, g);
}

void Mx50Fb::clear()
{
    ++m_clears;
    memset(m_fb, m_bgColor, m_fbSize);
}

void Mx50Fb::pset(int x, int y)
{
    *(((unsigned char*)m_fb) + y*vinfo.xres_virtual + x) = m_fgColor;
}

inline void Mx50Fb::hline(int x1, int y, int x2)
{
    int dx = x2 - x1;
    if (dx < 0)
        dx = -dx;
    memset(m_fb + y*vinfo.xres_virtual + x1, m_fgColor, dx+1);
}

void Mx50Fb::fillRect(Rect* r)
{
    int y2 = r->y + r->h - 1;
    for (int y = r->y; y < y2; ++y) {
        hline(r->x, y, r->x+r->w-1);
    }
}

static void invcpy(unsigned char *dst, unsigned char *src, size_t n)
{
#if 0
    // Assuming src is word aligned.
    // dst may not be aligned.

#else
    // TODO  invert then memcpy is total hack.  do it right:
    // respect alignment, invert while copying.  (Or is it possible
    // to have FreeType output inverted?)
    for (size_t i = 0; i < n; ++i) {
        src[i] = ~src[i];
    }
    memcpy(dst, src, n);
#endif
}

void Mx50Fb::blit(unsigned char *p, int x, int y, int w, int h)
{
    clc::Log::trace("ocher.mx50", "blit");
    const int fbw = width();
    if (x >= fbw)
        return;
    const int fbh = height();
    if (y >= fbh)
        return;
    if (x < 0) {
        // TODO
    }
    if (x + w >= fbw)
        w = fbw - x;
    if (y < 0) {
        // TODO
    }
    if (y + h >= fbh)
        h = fbh - y;

    for (int i = 0; i < h; ++i) {
        invcpy(((unsigned char*)m_fb) + y*vinfo.xres_virtual + x, p, w);
        y++;
        p = p + w;
    }
}

int Mx50Fb::update(Rect* r, bool full)
{
    Rect _r;
    if (!r) {
        _r.x = _r.y = 0;
        _r.w = width();
        _r.h = height();
        r = &_r;
    }
    if (m_clears > settings.fullRefreshPages) {
        m_clears = 0;
        full = true;
    }

    struct mxcfb_update_data region;

    region.update_region.left = r->x;
    region.update_region.top = r->y;
    region.update_region.width = r->w;
    region.update_region.height = r->h;
    region.waveform_mode = WAVEFORM_MODE_AUTO;
    region.update_mode = full ? UPDATE_MODE_FULL : UPDATE_MODE_PARTIAL;
    region.update_marker = ++m_marker;
    region.temp = TEMP_USE_AMBIENT;
    region.flags = 0;

    if (ioctl(m_fd, MXCFB_SEND_UPDATE, &region) == -1) {
        clc::Log::error("ocher.mx50", "MXCFB_SEND_UPDATE(%d, %d, %d, %d, %d): %s",
                r->x, r->y, r->w, r->h, m_marker, strerror(errno));
    }
    return m_marker;
}

void Mx50Fb::waitUpdate(int marker)
{
    if (ioctl(m_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &marker) == -1) {
        clc::Log::error("ocher.mx50", "MXCFB_WAIT_FOR_UPDATE_COMPLETE(%d): %s", marker, strerror(errno));
    }
}

void Mx50Fb::setPixelFormat()
{
    fb_var_screeninfo screen_info;
    screen_info.bits_per_pixel = 8;
    screen_info.grayscale = GRAYSCALE_8BIT;
    int retval = ioctl(m_fd, FBIOPUT_VSCREENINFO, &screen_info);
    if (retval)
        clc::Log::error("ocher.mx50", "FBIOPUT_VSCREENINFO: %s", strerror(errno));
}

void Mx50Fb::setAutoUpdateMode(bool autoUpdate)
{
    uint32_t mode;
    mode = autoUpdate ? AUTO_UPDATE_MODE_AUTOMATIC_MODE : AUTO_UPDATE_MODE_REGION_MODE;
    int retval = ioctl(m_fd, MXCFB_SET_AUTO_UPDATE_MODE, &mode);
    if (retval)
        clc::Log::error("ocher.mx50", "MXCFB_SET_AUTO_UPDATE_MODE(%d): %s", mode, strerror(errno));
}

void Mx50Fb::setUpdateScheme()
{
    fb_var_screeninfo screen_info;
//    screen_info.scheme = UPDATE_SCHEME_SNAPSHOT;
    int retval = ioctl(m_fd, MXCFB_SET_UPDATE_SCHEME, &screen_info);
    if (retval)
        clc::Log::error("ocher.mx50", "MXCFB_SET__UPDATE_SCHEME: %s", strerror(errno));
}

