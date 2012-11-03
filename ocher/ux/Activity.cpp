#include "ocher/ux/Factory.h"
#include "ocher/ux/Activity.h"


OcherCanvas::OcherCanvas()
{
}

OcherCanvas::~OcherCanvas()
{
}

void OcherCanvas::draw(Pos*)
{
    g_fb->setFg(0x80, 0x80, 0x80);
    g_fb->fillRect(&m_rect);
}

