#include "clc/support/Logger.h"

#include "ocher/settings/Settings.h"
#include "ocher/shelf/Meta.h"
#include "ocher/ux/Factory.h"
#include "ocher/ux/LibraryActivity.h"
#include "ocher/ux/fb/FontEngine.h"
#include "ocher/ux/fb/Widgets.h"

#define LOG_NAME "ocher.ux.Library"


class LibraryCanvas : public Canvas
{
public:
    LibraryCanvas(UiBits& ui);
    ~LibraryCanvas();

    int evtKey(struct OcherEvent*);
    int evtMouse(struct OcherEvent*);

    Rect draw(Pos* pos);

    Activity m_nextActivity;

protected:
    UiBits& m_ui;
#define NUM_BOOKS 30
    Rect books[NUM_BOOKS];
    int itemHeight;
    int itemsPerPage;
    unsigned int pageNum;
};

LibraryCanvas::LibraryCanvas(UiBits& ui) :
    m_ui(ui)
{
}

LibraryCanvas::~LibraryCanvas()
{
}

int LibraryCanvas::evtKey(struct OcherEvent* evt)
{
    if (evt->subtype == OEVT_KEY_DOWN) {
        if (evt->key.key == OEVTK_HOME) {
            clc::Log::info(LOG_NAME, "home");
            m_nextActivity = ACTIVITY_HOME;
            return 0;
        }
        // TODO
    }
    return -1;
}

int LibraryCanvas::evtMouse(struct OcherEvent* evt)
{
    if (evt->subtype == OEVT_MOUSE1_UP) {
        for (unsigned int i = 0; i < NUM_BOOKS; i++) {
            Pos* pos = (Pos*)&evt->mouse;
            if (books[i].contains(pos)) {
                Meta* meta = (Meta*)g_shelf->m_meta.get(i);
                g_shelf->select(meta);
                m_nextActivity = ACTIVITY_READ;
                return 0;
            }
        }
    }
    return -1;
}

Rect LibraryCanvas::draw(Pos*)
{
    g_fb->setFg(0xff, 0xff, 0xff);
    g_fb->fillRect(&m_rect);
    g_fb->setFg(0, 0, 0);

    FontEngine fe;
    fe.setSize(10);
    fe.apply();

    Rect r = g_fb->bbox;
    Pos pos;
    pos.x = 0;
    pos.y = m_ui.m_systemBar.m_rect.y + settings.largeSpace;
    unsigned int N = g_shelf->m_meta.size();
    for (unsigned int i = 0; i < N && i < NUM_BOOKS; ++i) {
        Meta* meta = (Meta*)g_shelf->m_meta.get(i);

        books[i].x = settings.medSpace;
        books[i].w = r.w - settings.medSpace*2;
        books[i].y = pos.y;
        books[i].h = fe.m_cur.lineHeight;
        pos.x = books[i].x;
        pos.y = books[i].y + fe.m_cur.ascender;
        fe.renderString(meta->title.c_str(), meta->title.length(), &pos, &r, FE_XCLIP);
        pos.y = books[i].y + fe.m_cur.lineHeight;
    }

    return r;
}


Activity LibraryActivity::run(UiBits& ui)
{
    clc::Log::info(LOG_NAME, "run");

    LibraryCanvas c(ui);

    ui.m_systemBar.m_sep = false;
    ui.m_systemBar.m_title = "LIBRARY";
    ui.m_systemBar.show();
    c.addChild(ui.m_systemBar);

    c.refresh();
    g_loop->run(&c);

    return c.m_nextActivity;
}

