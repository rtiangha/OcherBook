#include <stdlib.h>
#include <string.h>

#include "clc/support/Logger.h"
#include "ocher/ux/fb/Widgets.h"
#include "ocher/ux/Factory.h"

#define LOG_NAME "ocher.widgets"

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
    Rect drawn;
    drawn.setInvalid();
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

int Widget::_onMouseEvent(struct OcherMouseEvent* evt)
{
    int r = -2;
    for (unsigned int i = 0; i < m_children.size(); ++i) {
        r = ((Widget*)m_children.get(i))->_onMouseEvent(evt);
        if (r != -2)
            break;
    }

    if (r == -2) {
        r = evtMouse(evt);
    }
    return r;
}

void Widget::onMouseEvent(struct OcherMouseEvent* evt)
{
    _onMouseEvent(evt);
}


Canvas::Canvas() :
    Widget(0, 0, g_fb->width(), g_fb->height())
{
}

void Canvas::loop()
{
    g_loop->mouseEvent.Connect(this, &Widget::onMouseEvent);

    g_loop->run();

    g_loop->mouseEvent.Disconnect(this, &Widget::onMouseEvent);
}


void Canvas::refresh(bool full)
{
    clc::Log::debug(LOG_NAME, "refresh");
    Rect drawn = draw(0);
    if (drawn.valid())
        g_fb->update(&drawn, full);
}

Rect Canvas::draw(Pos*)
{
    return drawChildren(m_rect.pos());
}


Panel::Panel() :
    Widget(0, 0, g_fb->width(), g_fb->height())
{
}

Rect Panel::draw(Pos*)
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
    Widget(x, y, w, h)
{
}

Button::~Button()
{
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
    if (m_label.length()) {
        FontEngine fe;
        fe.setSize(14);
        fe.apply();
        Pos pos;
        pos.x = 0; pos.y = m_rect.h / 2;
        fe.renderString(m_label.c_str(), m_label.length(), &pos, &m_rect, FE_XCENTER);
    }
}

int Button::evtKey(struct OcherKeyEvent*)
{
    return -2;
}

int Button::evtMouse(struct OcherMouseEvent*)
{
    return -2;
}


Rect Icon::draw(Pos* pos)
{
    g_fb->blit(bmp->bmp, pos->x + m_rect.x, pos->y + m_rect.y, bmp->w, bmp->h);
#if 0
    if (! m_isActive) {
        g_fb->byLine(&m_rect, lighten);
    }
#endif
    return m_rect;
}
