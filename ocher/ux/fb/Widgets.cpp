#include <stdlib.h>
#include <string.h>

#include "ocher/ux/fb/Widgets.h"
#include "ocher/ux/Factory.h"

static const unsigned int borderWidth = 2;
static const unsigned int roundRadius = 1;


Widget::Widget(int x, int y, unsigned int w, unsigned int h) :
    m_rect(x, y, w, h),
    m_parent(0)
{
}

Widget::~Widget()
{
    for (unsigned int i = 0; i < m_children.size(); ++i) {
        delete ((Widget*)m_children.get(i));
    }
}

void Widget::addChild(Widget* child)
{
    m_children.add(child);
}

void Widget::drawChildren()
{
    for (unsigned int i = 0; i < m_children.size(); ++i) {
        ((Widget*)m_children.get(i))->draw();
    }
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

void Canvas::draw()
{
    g_fb->setFg(128, 128, 128);
    g_fb->fillRect(&m_rect);
    drawChildren();
    g_fb->update(&m_rect, false);
}


Window::Window(int x, int y, unsigned int w, unsigned int h) :
    Widget(x, y, w, h),
    m_bgColor(0xffffffff),
    m_borderColor(0),
    m_borderWidth(borderWidth),
    m_flags(0),
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

void Window::draw()
{
    Rect rect(m_rect);
    if (m_borderWidth) {
        g_fb->setFg(0, 0, 0);
        g_fb->hline(rect.x, rect.y, rect.x+rect.w-1);
        g_fb->hline(rect.x, rect.y+rect.h-1, rect.x+rect.w-1);
        g_fb->vline(rect.x, rect.y, rect.y+rect.h-1);
        g_fb->vline(rect.x+rect.w-1, rect.y, rect.y+rect.h-1);
        rect.x += m_borderWidth;
        rect.y += m_borderWidth;
        rect.w -= m_borderWidth*2;
        rect.h -= m_borderWidth*2;
    }
    g_fb->setFg(0xff, 0xff, 0xff);
    g_fb->fillRect(&rect);
    if (m_flags & OWF_CLOSE) {
        g_fb->setFg(0, 0, 0);
        g_fb->line(rect.x + rect.w - 12, rect.y + 4, rect.x + rect.w - 4, rect.y + 12);
        g_fb->line(rect.x + rect.w - 4, rect.y + 4, rect.x + rect.w - 12, rect.y + 12);
    }
    draw(-1);
    drawChildren();
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

void Button::draw()
{
    g_fb->hline(m_rect.x+roundRadius, m_rect.y, m_rect.x+m_rect.w-1-roundRadius);
    g_fb->hline(m_rect.x+roundRadius, m_rect.y+m_rect.h-1, m_rect.x+m_rect.w-1-roundRadius);
    g_fb->vline(m_rect.x, m_rect.y+roundRadius, m_rect.y+m_rect.h-1-roundRadius);
    g_fb->vline(m_rect.x+m_rect.w-1, m_rect.y+roundRadius, m_rect.y+m_rect.h-1-roundRadius);
    // TODO label
    if (roundRadius <= 1) {
        return;
    }
    // TODO
}

int Button::evtKey(struct OcherEvent*)
{
    return -1;
}

int Button::evtMouse(struct OcherEvent*)
{
    return -1;
}

