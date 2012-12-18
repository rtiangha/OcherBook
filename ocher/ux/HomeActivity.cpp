#include "clc/support/Logger.h"

#include "ocher/settings/Settings.h"
#include "ocher/shelf/Meta.h"
#include "ocher/ux/Factory.h"
#include "ocher/ux/HomeActivity.h"
#include "ocher/ux/fb/FontEngine.h"
#include "ocher/ux/fb/Widgets.h"

#define LOG_NAME "ocher.ux.Home"


class HomeCanvas : public Canvas
{
public:
    HomeCanvas();
    ~HomeCanvas();

    int evtKey(struct OcherEvent*);
    int evtMouse(struct OcherEvent*);

    Rect draw(Pos* pos);

protected:
#define NUM_CLUSTER_BOOKS 5
    Rect books[NUM_CLUSTER_BOOKS];
    Rect shortlist[5];
    float coverRatio;
};

HomeCanvas::HomeCanvas() :
    coverRatio(1.6)
{
    int dx = m_rect.w/32;
    int dy = m_rect.h/36;

    books[0].x = m_rect.w/15;
    books[0].y = m_rect.h/5;
    books[0].w = m_rect.w/2.8;
    books[0].h = books[0].w*coverRatio;;

    books[1].x = books[0].x + books[0].w + dx;
    books[1].y = m_rect.h/6;
    books[1].w = m_rect.w/4;
    books[1].h = books[1].w*coverRatio;;

    books[2].x = books[1].x + books[1].w + dx;
    books[2].w = m_rect.w/5;
    books[2].h = books[2].w*coverRatio;;
    books[2].y = books[1].y + books[1].h - books[2].h;

    books[3].x = books[1].x;
    books[3].y = books[1].y + books[1].h + dy;
    books[3].w = books[2].w;
    books[3].h = books[2].h;

    books[4] = books[3];
    books[4].x += books[3].w + dx;
    books[4].w -= 2*dx;
    books[4].h = books[4].w*coverRatio;;
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
    if (evt->subtype == OEVT_MOUSE1_UP) {
        for (unsigned int i = 0; i < NUM_CLUSTER_BOOKS; i++) {
            Meta* meta = (Meta*)g_shelf->m_meta.itemAt(i);
            if (!meta) {
                clc::Log::debug(LOG_NAME, "book %d has no meta", i);
                continue;
            }
            Pos* pos = (Pos*)&evt->mouse;
            if (books[i].contains(pos)) {
                clc::Log::info(LOG_NAME, "book %d selected %p", i, meta);
                Rect r = books[i];
                r.inset(-2);
                g_fb->roundRect(&r, 3);
                r.inset(-1);
                g_fb->roundRect(&r, 4);
                g_fb->update(&r);
                g_shelf->select(meta);
                return ACTIVITY_READ;
            } else if (pos->y > 650 /* TODO convert to labels */) {
                // TODO:  For now, clicking on shortlist takes you to library.  Need popup menus.
                return ACTIVITY_LIBRARY;
            }
        }
        clc::Log::debug(LOG_NAME, "no book under click @ %d,%d", evt->mouse.x, evt->mouse.y);
    }
    return -1;
}

Rect HomeCanvas::draw(Pos*)
{
    Rect drawn;
    drawn.setInvalid();

    if (m_flags & WIDGET_DIRTY) {
        clc::Log::info(LOG_NAME, "draw");
        m_flags &= ~WIDGET_DIRTY;
        drawn = m_rect;

        g_fb->setFg(0xff, 0xff, 0xff);
        g_fb->fillRect(&m_rect);
        g_fb->setFg(0, 0, 0);

        FontEngine fe;
        fe.setSize(12);
        fe.apply();
        Rect r;
        Pos pos;
        for (unsigned int i = 0; i < NUM_CLUSTER_BOOKS; ++i) {
            r = books[i];
            r.inset(-1);
            g_fb->rect(&r);
            r.inset(-1);
            g_fb->roundRect(&r, 1);

            Meta* meta = (Meta*)g_shelf->m_meta.itemAt(i);
            if (! meta) {
                r.inset(2);
                g_fb->setFg(0xd0, 0xd0, 0xd0);
                g_fb->fillRect(&r);
                g_fb->setFg(0, 0, 0);
            } else {
                pos.x = 0;
                pos.y = fe.m_cur.ascender;
                r.inset(2);
                fe.renderString(meta->title.c_str(), meta->title.length(), &pos, &r, FE_XCLIP);
            }
        }

        fe.setSize(18);
        fe.apply();
        pos.x = 0; pos.y = 100;
        fe.renderString("HOME", 4, &pos, &m_rect, FE_XCENTER);

        // Shortlist
        fe.setSize(14);
        fe.apply();
        pos.x = books[0].x; pos.y = 650;
        fe.renderString("Shortlist", 9, &pos, &m_rect, 0);
        pos.y += fe.m_cur.descender + settings.smallSpace;
        g_fb->hline(books[0].x, pos.y, m_rect.w - books[0].x);
        pos.y++;
        g_fb->hline(books[0].x, pos.y, m_rect.w - books[0].x);
        {
            Rect sl;
            // TODO
        }

        // TODO simplify, abstract into labels
        {
            fe.setSize(12);
            fe.setItalic(1);
            fe.apply();
            Rect lbox;
            lbox.x = 0;
            lbox.y = 650;
            Glyph* glyphs[13];
            const char* text = "Browse all...";
            fe.plotString(text, strlen(text), &glyphs[0], &lbox);
            pos.x = m_rect.w - books[0].x - lbox.w;
            pos.y = lbox.y;
            fe.renderString(text, strlen(text), &pos, &m_rect, 0);
        }

    }
    Rect d2 = drawChildren(m_rect.pos());
    drawn.unionRect(&d2);

#if 0
    g_fb->byLine(&g_fb->bbox, dim);
    Rect popup(25, 200, 550, 400);
    g_fb->rect(&popup);
    popup.inset(1);
    g_fb->rect(&popup);
    popup.inset(1);
    g_fb->setFg(0xff, 0xff, 0xff);
    g_fb->fillRect(&popup);
#endif

    return drawn;
}


HomeActivity::HomeActivity()
{
}

Activity HomeActivity::run(UiBits& ui)
{
    clc::Log::info(LOG_NAME, "run");
    HomeCanvas c;

    ui.m_systemBar.m_sep = false;
    ui.m_systemBar.m_title.clear();
    ui.m_systemBar.show();
    c.addChild(ui.m_systemBar);

    c.refresh(true);
    return (Activity)g_loop->run(&c);
}

