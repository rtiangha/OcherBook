#ifndef OCHER_CURSES_RENDER_H
#define OCHER_CURSES_RENDER_H

#include <cdk/cdk.h>

#include "ocher/ux/Renderer.h"


class RenderCurses : public Renderer
{
public:
    RenderCurses();

    bool init(CDKSCREEN* screen);
    int render(unsigned int pageNum, bool doBlit);

    int outputWrapped(clc::Buffer *b, unsigned int strOffset, bool doBlit);

protected:
    CDKSCREEN* m_screen;
    CDKSWINDOW* m_window;
    int m_width;
    int m_height;
    int m_x;
    int m_y;
    int m_page;

    void enableUl();
    void disableUl();
    void enableEm();
    void disableEm();

    void pushAttrs();
    void applyAttrs(int i);
    void popAttrs();

    Attrs a[10];
    int ai;
};

#endif

