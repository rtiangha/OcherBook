#include "ocher/ux/ncurses/FactoryNC.h"
#include "ocher/ocher.h"


UX_DRIVER_REGISTER(Curses);


UiFactoryCurses::UiFactoryCurses() :
    m_browser(0),
    m_renderer(0)
{
}

bool UiFactoryCurses::init()
{
    m_browser = new BrowseCurses();
    m_renderer = new RenderCurses();

    m_scr = initscr();
    m_screen = initCDKScreen(m_scr);
    initCDKColor();
    m_browser->init(m_screen);
    m_renderer->init(m_screen);

    return true;
}

void UiFactoryCurses::deinit()
{
    delete m_browser;
    m_browser = 0;
    delete m_renderer;
    m_renderer = 0;
    destroyCDKScreen(m_screen);
    endCDK();
    delwin(m_scr);
}

const char* UiFactoryCurses::getName()
{
    return "ncurses";
}

Browse* UiFactoryCurses::getBrowser()
{
    return m_browser;
}

Renderer* UiFactoryCurses::getRenderer()
{
    return m_renderer;
}


