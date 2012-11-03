#include "ocher/ux/Factory.h"
#include "ocher/ux/fb/Widgets.h"
#include "ocher/ux/HomeActivity.h"


class HomeCanvas : public Canvas
{
public:
    HomeCanvas();
    ~HomeCanvas();

    int evtKey(struct OcherEvent*);
    int evtMouse(struct OcherEvent*);

    void draw(Pos* pos);

protected:
#define NUM_CLUSTER_BOOKS 5
    Rect books[NUM_CLUSTER_BOOKS];
    Meta* meta[NUM_CLUSTER_BOOKS];
};

HomeCanvas::HomeCanvas()
{
    books[0].x = m_rect.w/12;
    books[0].y = m_rect.h/5;
    books[0].w = m_rect.w/3;
    books[0].h = m_rect.h/2.5;

    for (unsigned int i = 1; i < NUM_CLUSTER_BOOKS; ++i) {
        meta[i] = 0;
    }

}

HomeCanvas::~HomeCanvas()
{
}

int HomeCanvas::evtKey(struct OcherEvent*)
{
    return -1;
}

int HomeCanvas::evtMouse(struct OcherEvent* evt)
{
    if (evt->subtype == OEVT_MOUSE1_CLICKED) {
        for (unsigned int i = 0; i < NUM_CLUSTER_BOOKS; i++) {
            if (!meta[i])
                continue;
            if (books[i].contains((Pos*)&evt->mouse)) {
                //TODO: g_cont->focus(meta[i]);
                return 0;
            }
        }
    }
    return -1;
}

void HomeCanvas::draw(Pos*)
{
    g_fb->setFg(0xff, 0xff, 0xff);
    g_fb->fillRect(&m_rect);

    g_fb->setFg(0, 0, 0);
    g_fb->roundRect(&books[0], 1);

    drawChildren(m_rect.pos());
    g_fb->update(&m_rect, false);
}


HomeActivity::HomeActivity()
{
}

Activity HomeActivity::run()
{
    HomeCanvas c;
    c.addChild(&g_cont->m_systemBar);
    c.draw(0);
    int r = g_loop->run(&c);

    return ACTIVITY_READ;
}



