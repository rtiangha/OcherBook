/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_UXCONTROLLER_H
#define OCHER_UX_FB_UXCONTROLLER_H

#include "ux/Controller.h"
#include "ux/fb/ActivityFb.h"

class UxControllerFb : public UxController {
public:
    UxControllerFb();
    ~UxControllerFb() = default;

    const char* getName() const override
    {
        return m_name.c_str();
    }

    bool init() override;

    FrameBuffer* getFrameBuffer() override
    {
        return m_frameBuffer.get();
    }

    FontEngine* getFontEngine() override
    {
        return m_fontEngine.get();
    }

    Renderer* getRenderer() override
    {
        return m_renderer.get();
    }

    void setNextActivity(Activity::Type a) override;

protected:
    std::string m_name;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<FontEngine> m_fontEngine;
    std::unique_ptr<FrameBuffer> m_frameBuffer;
    FbScreen m_screen;
    ActivityFb* m_activity = nullptr;
};

#endif
