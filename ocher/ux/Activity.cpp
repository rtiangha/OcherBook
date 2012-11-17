#include "ocher/ux/Factory.h"
#include "ocher/ux/Activity.h"


OcherCanvas::OcherCanvas()
{
}

OcherCanvas::~OcherCanvas()
{
}

Rect OcherCanvas::draw(Pos*)
{
    Rect drawn;
    if (m_flags & WIDGET_DIRTY) {
        m_flags &= ~WIDGET_DIRTY;
        g_fb->setFg(0x80, 0x80, 0x80);
        g_fb->fillRect(&m_rect);
        drawn = m_rect;
    } else {
        drawn.setInvalid();
    }
    return drawn;
}

