#include <unistd.h>

#include "clc/support/Logger.h"
#include "ocher/device/Device.h"
#include "ocher/ux/Factory.h"
#include "ocher/ux/Activity.h"

#define LOG_NAME "ocher.ux.Read"


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

int OcherCanvas::evtKey(struct OcherEvent* evt)
{
    if (evt->subtype == OEVT_KEY_DOWN) {
        if (evt->key.key == OEVTK_POWER) {
            FontEngine fe;
            Rect r = g_fb->bbox;
            g_fb->clear();

            fe.setSize(18);
            fe.setItalic(1);
            fe.apply();

            Pos p;
            p.x = 0;
            p.y = r.h/2;
            fe.renderString("Sleeping", 8, &p, &r, FE_XCENTER);

            g_fb->update(&r, true);  // Full refresh to clear page remnants
            g_fb->sync();
            sleep(1); // TODO seems hackish but sync doesn't wait long enough!

            device->sleep();

            g_fb->clear();
            dirty();

            return -1;
        }
    }
    return -2;
}

int OcherCanvas::evtMouse(struct OcherEvent*)
{
    return -2;
}

int OcherCanvas::evtApp(struct OcherEvent* evt)
{
    if (evt->subtype == OEVT_APP_CLOSE) {
        return ACTIVITY_QUIT;
    }
    return -2;
}

int OcherCanvas::evtDevice(struct OcherEvent*)
{
    return -2;
}
