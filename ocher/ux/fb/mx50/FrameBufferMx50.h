/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef MX50_FB_H
#define MX50_FB_H

#include "ux/fb/FbLinux.h"


class FrameBufferMx50 : public FbLinux {
public:
    FrameBufferMx50() = default;
    ~FrameBufferMx50() = default;

    bool init() override;
    void initVarInfo() override;

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
    unsigned int m_marker = 0;
    bool m_needFull = false;
};

#endif
