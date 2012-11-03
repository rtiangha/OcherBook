#ifndef OCHER_UX_FACTORY_NC_H
#define OCHER_UX_FACTORY_NC_H

#include "ocher/ux/Factory.h"
#include "ocher/ux/ncurses/RenderCurses.h"

class UiFactoryCurses : public UiFactory
{
public:
    UiFactoryCurses();
    virtual ~UiFactoryCurses() {}

    bool init();
    void deinit();

    const char* getName();
    Renderer* getRenderer();

protected:
    RenderCurses* m_renderer;
    WINDOW* m_scr;
    CDKSCREEN *m_screen;
};

#endif

