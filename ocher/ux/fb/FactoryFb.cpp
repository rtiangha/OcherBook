#include "ocher/ux/fb/FactoryFb.h"
#include "ocher/ocher.h"


UiFactoryFb::UiFactoryFb() :
    m_fb(0),
    m_ft(0),
    m_browser(0),
    m_render(0)
{
}

bool UiFactoryFb::init()
{
    // Derived init must have set m_fb.
    m_ft = new FreeType(m_fb);
    if (m_ft->init()) {
        m_render = new RenderFb(m_ft, m_fb);
        if (m_render->init()) {
            m_browser = new BrowseFb();
            if (m_browser->init()) {
                return true;
            }
        }
    }
    deinit();
    return false;
}

void UiFactoryFb::deinit()
{
}

Browse* UiFactoryFb::getBrowser()
{
    return m_browser;
}

Renderer* UiFactoryFb::getRenderer()
{
    return m_render;
}



