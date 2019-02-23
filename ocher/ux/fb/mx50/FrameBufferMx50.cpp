/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/mx50/FrameBufferMx50.h"

#include "Container.h"
#include "device/Device.h"
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

#define uint unsigned int
#include <linux/mxcfb.h>

#define LOG_NAME "ocher.mx50"

bool FrameBufferMx50::init()
{
    if (!FbLinux::init())
        return false;

    update(nullptr, true);
    return true;
}

void FrameBufferMx50::initVarInfo()
{
    // The mxc_epdc_fb.c driver doesn't report physical size.
    if (vinfo.width == (unsigned)-1 && vinfo.height == (unsigned)-1) {
        const auto& device = g_container->device;
        int xres = device->screenattrs[Device::XRes];
        int yres = device->screenattrs[Device::YRes];
        int ppi  = device->screenattrs[Device::PPI];
        vinfo.width = std::ceil(xres * 25.4 / ppi);
        vinfo.height = std::ceil(yres * 25.4 / ppi);
    }

    vinfo.bits_per_pixel = 8;
    vinfo.grayscale = 1;
    vinfo.red = vinfo.green = vinfo.blue = vinfo.transp = { 0, 8, 0 };
    // 0 is landscape right handed, 1 portait upside-down, 2 , 3 portrait
    vinfo.rotate = 3;
}

unsigned int FrameBufferMx50::ppi()
{
    const auto& device = g_container->device;
    return device->screenattrs[Device::PPI];
}

int FrameBufferMx50::update(const Rect* rect, bool full)
{
    Rect r;

    const int width = xres();
    const int height = yres();
    if (!rect) {
        r.x = r.y = 0;
        r.w = width;
        r.h = height;
    } else {
        r = *rect;

        if (r.x + r.w > width) {
            if (r.x >= width)
                return m_marker;
            r.w = width - r.x;
        }
        if (r.y + r.h > height) {
            if (r.y >= height)
                return m_marker;
            r.h = height - r.y;
        }
    }

    if (m_needFull) {
        m_needFull = false;
        full = true;
    }
    Log::info(LOG_NAME, "update %d %d %u %u", r.x, r.y, r.w, r.h);
    struct mxcfb_update_data region;

    region.update_region.left = r.x;
    region.update_region.top = r.y;
    region.update_region.width = r.w;
    region.update_region.height = r.h;
    region.waveform_mode = WAVEFORM_MODE_AUTO;
    region.update_mode = full ? UPDATE_MODE_FULL : UPDATE_MODE_PARTIAL;
    region.update_marker = ++m_marker;
    region.temp = TEMP_USE_AMBIENT;
    region.flags = 0;

    if (ioctl(m_fd, MXCFB_SEND_UPDATE, &region) == -1) {
        Log::error(LOG_NAME, "MXCFB_SEND_UPDATE(%d, %d, %d, %d, %d): %s",
                r.x, r.y, r.w, r.h, m_marker, strerror(errno));
    } else {
        Log::info(LOG_NAME, "update %d started", m_marker);
    }

    return m_marker;
}

void FrameBufferMx50::sync()
{
    waitUpdate(m_marker);
}

void FrameBufferMx50::waitUpdate(int marker)
{
    if (ioctl(m_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &marker) == -1) {
        Log::error(LOG_NAME, "MXCFB_WAIT_FOR_UPDATE_COMPLETE(%d): %s", marker, strerror(errno));
    } else {
        Log::info(LOG_NAME, "update %d done", marker);
    }
}

void FrameBufferMx50::setPixelFormat()
{
    struct fb_var_screeninfo fb_var = vinfo;
    fb_var.bits_per_pixel = 8;
    fb_var.grayscale = GRAYSCALE_8BIT;
    int retval = ioctl(m_fd, FBIOPUT_VSCREENINFO, &fb_var);
    if (retval)
        Log::error(LOG_NAME, "FBIOPUT_VSCREENINFO: %s", strerror(errno));
}

void FrameBufferMx50::setAutoUpdateMode(bool autoUpdate)
{
    uint32_t mode;

    mode = autoUpdate ? AUTO_UPDATE_MODE_AUTOMATIC_MODE : AUTO_UPDATE_MODE_REGION_MODE;
    int retval = ioctl(m_fd, MXCFB_SET_AUTO_UPDATE_MODE, &mode);
    if (retval)
        Log::error(LOG_NAME, "MXCFB_SET_AUTO_UPDATE_MODE(%d): %s", mode, strerror(errno));
}

void FrameBufferMx50::setUpdateScheme()
{
#if 0
    vinfo.scheme = UPDATE_SCHEME_SNAPSHOT;
    int retval = ioctl(m_fd, MXCFB_SET_UPDATE_SCHEME, &vinfo);
    if (retval)
        Log::error(LOG_NAME, "MXCFB_SET__UPDATE_SCHEME: %s", strerror(errno));
#endif
}
