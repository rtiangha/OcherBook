#include "ocher/ux/fb/Widgets.h"
#include "ocher/ux/fb/ClockIcon.h"

ClockIcon::ClockIcon(int x, int y) :
    Widget(x, y, 30, 30),
    m_active(false)
{
}

Rect ClockIcon::draw(Pos* pos)
{
    Rect rect(m_rect);
    rect.offsetBy(pos);

    // TODO

    return m_rect;
}

void ClockIcon::onActivate(bool active)
{
    m_active = active;
}

