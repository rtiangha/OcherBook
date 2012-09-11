#ifndef OCHER_UX_FACTORY_FB_H
#define OCHER_UX_FACTORY_FB_H

#include "ocher/ux/Factory.h"
#include "ocher/device/Event.h"
#include "ocher/ux/fb/BrowseFb.h"
#include "ocher/ux/fb/FreeType.h"
#include "ocher/ux/fb/RenderFb.h"


class UiFactoryFb : public UiFactory
{
public:
    UiFactoryFb();
    virtual ~UiFactoryFb() {}

    bool init();
    void deinit();

    Browse* getBrowser() { return m_browser; }
    Renderer* getRenderer() { return m_render; }
    EventLoop* getLoop() { return m_loop; }
    FrameBuffer* getFrameBuffer() { return m_fb; }

protected:
    FrameBuffer* m_fb;
    FreeType* m_ft;
    BrowseFb* m_browser;
    RenderFb* m_render;
    EventLoop* m_loop;
};

#endif

