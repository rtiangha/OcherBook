#include <stdlib.h>
#include <string.h>

#include "ocher/ux/fb/Widgets.h"
#include "ocher/ux/Factory.h"

static const unsigned int borderWidth = 2;
static const unsigned int roundRadius = 1;


Widget::Widget(int x, int y, unsigned int w, unsigned int h) :
    m_rect(x, y, w, h),
    m_flags(WIDGET_DIRTY),
    m_parent(0)
{
}

Widget::~Widget()
{
    for (unsigned int i = 0; i < m_children.size(); ++i) {
        Widget* w = (Widget*)m_children.get(i);
        if (w->m_flags & WIDGET_OWNED)
            delete w;
    }
}

void Widget::addChild(Widget* child)
{
    child->m_flags |= WIDGET_OWNED;
    m_children.add(child);
    if (! (child->m_flags & WIDGET_HIDDEN)) {
        dirty();
    }
}

void Widget::addChild(Widget& child)
{
    m_children.add(&child);
    if (! (child.m_flags & WIDGET_HIDDEN)) {
        dirty();
    }
}

void Widget::dirty()
{
    m_flags |= WIDGET_DIRTY;
    for (unsigned int i = 0; i < m_children.size(); ++i) {
        Widget* w = (Widget*)m_children.get(i);
        w->dirty();
    }
}

Rect Widget::drawChildren(Pos* pos)
{
    const size_t N = m_children.size();
    Rect drawn(-1, -1, 0, 0);
    if (N) {
        for (unsigned int i = 0; i < N; ++i) {
            Widget* w = (Widget*)m_children.get(i);
           if (! (w->m_flags & WIDGET_HIDDEN)) {
                Rect r = w->draw(pos);
                drawn.unionRect(&r);
           }
        }
    }
    return drawn;
}

int Widget::eventReceived(struct OcherEvent* evt)
{
    int r;
    for (unsigned int i = 0; i < m_children.size(); ++i) {
        // TODO: filter mouse by child coords, keys by focus, etc
        r = ((Widget*)m_children.get(i))->eventReceived(evt);
        if (r == 0)
            return 0;
    }
    r = -1;
    switch (evt->type) {
        case OEVT_KEY: {
            r = evtKey(evt);
            break;
        }
        case OEVT_MOUSE: {
            r = evtMouse(evt);
            break;
        }
        case OEVT_APP: {
            r = evtApp(evt);
            break;
        }
        case OEVT_DEVICE: {
            r = evtDevice(evt);
            break;
        }
    }
    return r;
}


Canvas::Canvas() :
    Widget(0, 0, g_fb->width(), g_fb->height())
{
}

void Canvas::refresh()
{
    Rect drawn = draw(0);
    if (drawn.valid())
        g_fb->update(&drawn);
}

Rect Canvas::draw(Pos*)
{
    Rect drawn;
    if (m_flags & WIDGET_DIRTY) {
        m_flags &= ~WIDGET_DIRTY;
        drawn = m_rect;
        g_fb->setFg(128, 128, 128);
        g_fb->fillRect(&m_rect);
        drawChildren(m_rect.pos());
    } else {
        drawn = drawChildren(m_rect.pos());
    }
    return drawn;
}


Window::Window(int x, int y, unsigned int w, unsigned int h) :
    Widget(x, y, w, h),
    m_bgColor(0xffffffff),
    m_borderColor(0),
    m_borderWidth(borderWidth),
    m_winflags(0),
    m_title(0)
{
}

Window::~Window()
{
    if (m_title)
        free(m_title);
}

void Window::setTitle(const char* title)
{
    if (m_title)
        free(m_title);
    m_title = strdup(title);
}

Rect Window::draw(Pos* pos)
{
    Rect rect(m_rect);
    rect.offsetBy(pos);
    Rect drawn;
    if (m_flags & WIDGET_DIRTY) {
        m_flags &= ~WIDGET_DIRTY;
        drawn = rect;
        drawBorder(&rect);
        drawTitle(&rect);
        drawBg(&rect);
        drawContent(&rect);
        drawChildren(rect.pos());
    } else {
        drawn = drawChildren(rect.pos());
    }
    return drawn;
}

void Window::drawBorder(Rect* rect)
{
    if (m_borderWidth) {
        g_fb->setFg(0, 0, 0);
        g_fb->rect(rect);
        rect->x += m_borderWidth;
        rect->y += m_borderWidth;
        rect->w -= m_borderWidth*2;
        rect->h -= m_borderWidth*2;
    }
}

void Window::drawTitle(Rect* rect)
{
    if (m_winflags & OWF_CLOSE) {
        g_fb->setFg(0, 0, 0);
        g_fb->line(rect->x + rect->w - 12, rect->y + 4, rect->x + rect->w - 4, rect->y + 12);
        g_fb->line(rect->x + rect->w - 4, rect->y + 4, rect->x + rect->w - 12, rect->y + 12);
        rect->y += 12;
        rect->h -= 12;
    }
}

void Window::drawBg(Rect* rect)
{
    g_fb->setFg(0xff, 0xff, 0xff);
    g_fb->fillRect(rect);
}

void Window::drawContent(Rect*)
{
}


Button::Button(int x, int y, unsigned int w, unsigned int h) :
    Widget(x, y, w, h),
    m_label(0)
{
}

Button::~Button()
{
    if (m_label)
        free(m_label);
}

Rect Button::draw(Pos* pos)
{
    Rect rect(m_rect);
    rect.offsetBy(pos);
    Rect drawn;
    if (m_flags & WIDGET_DIRTY) {
        m_flags &= ~WIDGET_DIRTY;
        drawn = rect;
        drawBorder(&rect);
        drawLabel(&rect);
        drawChildren(rect.pos());
    } else {
        drawn = drawChildren(rect.pos());
    }
    return drawn;
}

void Button::drawBorder(Rect* rect)
{
    g_fb->roundRect(rect, roundRadius);
}

void Button::drawLabel(Rect* rect)
{
    if (m_label) {
        // TODO
    }
}

int Button::evtKey(struct OcherEvent*)
{
    return -1;
}

int Button::evtMouse(struct OcherEvent*)
{
    return -1;
}


Icon::Icon(int x, int y, unsigned int w, unsigned int h) :
    Widget(x, y, w, h)
{
}

