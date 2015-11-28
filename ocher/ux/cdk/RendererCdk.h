/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_CDK_RENDERER_H
#define OCHER_UX_CDK_RENDERER_H

#include "ocher/ux/Renderer.h"

#include <cdk/cdkscreen.h>

class Pagination;

class RendererCdk : public Renderer {
public:
    RendererCdk();

    bool init(WINDOW *scr, CDKSCREEN *m_screen);
    int render(Pagination *pagination, unsigned int pageNum, bool doBlit);

    int outputWrapped(std::string *b, unsigned int strOffset, bool doBlit);

protected:
    WINDOW *m_scr;
    CDKSCREEN *m_screen;
    int m_width;
    int m_height;
    int m_x;
    int m_y;

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
