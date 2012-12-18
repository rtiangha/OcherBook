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
    LibraryCanvas(UiBits& ui, clc::List& meta);
    ~LibraryCanvas();

    int evtKey(struct OcherEvent*);
    int evtMouse(struct OcherEvent*);

    Rect draw(Pos* pos);

protected:
    UiBits& m_ui;
    clc::List m_meta;
#define BOOKS_PER_PAGE 15
    Rect* m_bookRects;
    int itemHeight;
    unsigned int m_booksPerPage;
    unsigned int m_pages;
    unsigned int m_pageNum;
};

LibraryCanvas::LibraryCanvas(UiBits& ui, clc::List& meta) :
    m_ui(ui),
    m_meta(meta),
    m_pageNum(0)
{
    // TODO calc this
    m_booksPerPage = BOOKS_PER_PAGE;
    m_bookRects = new Rect[m_booksPerPage];
    m_pages = (m_meta.size() + m_booksPerPage - 1) / m_booksPerPage;
}

LibraryCanvas::~LibraryCanvas()
{
    delete[] m_bookRects;
}

int LibraryCanvas::evtKey(struct OcherEvent* evt)
{
    if (evt->subtype == OEVT_KEY_DOWN) {
        if (evt->key.key == OEVTK_HOME) {
            clc::Log::info(LOG_NAME, "home");
            return ACTIVITY_HOME;
        } else if (evt->key.key == OEVTK_LEFT || evt->key.key == OEVTK_UP || evt->key.key == OEVTK_PAGEUP) {
            clc::Log::info(LOG_NAME, "back from page %d", m_pageNum);
            if (m_pageNum > 0) {
                m_pageNum--;
                dirty();
            }
            return -1;
        } else if (evt->key.key == OEVTK_RIGHT || evt->key.key == OEVTK_DOWN || evt->key.key == OEVTK_PAGEDOWN) {
            clc::Log::info(LOG_NAME, "forward from page %d", m_pageNum);
            if (m_pageNum+1 < m_pages) {
                m_pageNum++;
                dirty();
            }
            return -1;
        }
    }
    return -2;
}

int LibraryCanvas::evtMouse(struct OcherEvent* evt)
{
    if (evt->subtype == OEVT_MOUSE1_UP) {
        Pos* pos = (Pos*)&evt->mouse;
        for (unsigned int i = 0; i < m_booksPerPage; i++) {
            Meta* meta = (Meta*)m_meta.get(i + m_pageNum*m_booksPerPage);
            if (!meta)
                break;
            if (m_bookRects[i].contains(pos)) {
                g_shelf->select(meta);
                return ACTIVITY_READ;
            }
        }
        // TODO buttons
        if (pos->x < 300) {
            clc::Log::info(LOG_NAME, "back from page %d", m_pageNum);
            if (m_pageNum > 0) {
                m_pageNum--;
                dirty();
            }
        } else {
            clc::Log::info(LOG_NAME, "forward from page %d", m_pageNum);
            if (m_pageNum+1 < m_pages) {
                m_pageNum++;
                dirty();
            }
        }
    }
    return -1;
}

Rect LibraryCanvas::draw(Pos*)
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
        Rect r = g_fb->bbox;
        Pos pos;

        fe.setSize(10);
        fe.apply();
        clc::Buffer str;
        str.format("PG. %u OF %u", m_pageNum+1, m_pages);
        pos.x = 0;
        pos.y = r.h - settings.smallSpace - fe.m_cur.descender;
        fe.renderString(str.c_str(), str.length(), &pos, &r, FE_XCENTER);
        int maxY = pos.y - fe.m_cur.ascender;

        pos.x = 0;
        pos.y = m_ui.m_systemBar.m_rect.y + settings.largeSpace;
        for (unsigned int i = 0; i < m_booksPerPage; ++i) {
            Meta* meta = (Meta*)m_meta.get(i + m_pageNum*m_booksPerPage);
            if (!meta)
                break;

            // TODO... br, # pages, type, ...

            m_bookRects[i].x = settings.medSpace;
            m_bookRects[i].w = r.w - settings.medSpace*2;
            m_bookRects[i].y = pos.y;
            m_bookRects[i].h = fe.m_cur.lineHeight*2;
            fe.setSize(12);
            fe.apply();
            pos.x = m_bookRects[i].x;
            pos.y = m_bookRects[i].y + 5 + fe.m_cur.ascender;
            fe.renderString(meta->title.c_str(), meta->title.length(), &pos, &r, FE_XCLIP);
            pos.x = 0;
            pos.y += fe.m_cur.lineHeight;
            fe.setSize(10);
            fe.apply();
            str.format("%u%% read", meta->percentRead());
            fe.renderString(str.c_str(), str.length(), &pos, &r, FE_XCLIP);
        }
    }

    return drawn;
}


Activity LibraryActivity::run(UiBits& ui)
{
    clc::Log::info(LOG_NAME, "run");

    LibraryCanvas c(ui, g_shelf->m_meta);

    ui.m_systemBar.m_sep = false;
    ui.m_systemBar.m_title = "LIBRARY";
    ui.m_systemBar.show();
    c.addChild(ui.m_systemBar);

    int r;
    while (1) {
        c.refresh();

        struct OcherEvent evt;
        g_fb->sync();
        g_loop->flush();
        r = g_loop->wait(&evt);
        if (r == 0) {
            r = c.eventReceived(&evt);
            if (r >= 0)
                break;
        }
    }

    return (Activity)r;
}

