#include "ocher/ux/fd/FactoryFd.h"
#include "ocher/ocher.h"


UX_DRIVER_REGISTER(Fd);


UiFactoryFd::UiFactoryFd() :
    m_browser(0),
    m_renderer(0)
{
}

bool UiFactoryFd::init()
{
    m_browser = new BrowseFd();
    m_renderer = new RendererFd();

    if (m_renderer->init() && m_browser->init())
        return true;
    deinit();
    return false;
}

void UiFactoryFd::deinit()
{
    delete m_browser;
    m_browser = 0;
    delete m_renderer;
    m_renderer = 0;
}

const char* UiFactoryFd::getName()
{
    return "fd";
}

Browse* UiFactoryFd::getBrowser()
{
    return m_browser;
}

Renderer* UiFactoryFd::getRenderer()
{
    return m_renderer;
}

