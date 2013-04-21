#include "ocher/ux/fb/Widgets.h"
#include "ocher/ux/fb/ClockIcon.h"

ClockIcon::ClockIcon(int x, int y) :
    Widget(x, y, 30, 30),
    m_active(false)
{
}

void ClockIcon::draw()
{
    // TODO
}

void ClockIcon::onActivate(bool active)
{
    m_active = active;
}

