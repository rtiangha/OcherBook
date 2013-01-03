#include "clc/support/Logger.h"

#include "ocher/settings/Settings.h"
#include "ocher/shelf/Meta.h"
#include "ocher/ux/Factory.h"
#include "ocher/ux/LibraryActivity.h"
#include "ocher/ux/fb/FontEngine.h"
#include "ocher/ux/fb/Widgets.h"
#include "ocher/resources/Bitmaps.h"

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
#define BOOKS_PER_PAGE 12
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

    ui.m_systemBar.m_sep = false;
    ui.m_systemBar.m_title = "LIBRARY";
    ui.m_systemBar.show();
    addChild(ui.m_systemBar);
    addChild(new Icon(settings.medSpace + settings.largeSpace,
                m_rect.h - settings.medSpace - bmpLeftArrow.h, &bmpLeftArrow));
    addChild(new Icon(m_rect.w - (settings.medSpace + settings.largeSpace + bmpRightArrow.w),
                m_rect.h - settings.medSpace - bmpRightArrow.h, &bmpRightArrow));

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
                g_library->select(meta);
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
        clc::Buffer str;
        Pos pos;
        Rect clip = g_fb->bbox;

        fe.setSize(10);
        fe.apply();
        str.format("PG. %u OF %u", m_pageNum+1, m_pages);
        pos.x = 0;
        pos.y = clip.h - settings.smallSpace - fe.m_cur.descender;
        fe.renderString(str.c_str(), str.length(), &pos, &clip, FE_XCENTER);
// TODO        int maxY = pos.y - fe.m_cur.ascender;

        clip.y = m_ui.m_systemBar.m_rect.y + m_ui.m_systemBar.m_rect.h + settings.medSpace;
        clip.x = settings.medSpace;
        clip.w -= settings.medSpace*2;
        for (unsigned int i = 0; i < m_booksPerPage; ++i) {
            Meta* meta = (Meta*)m_meta.get(i + m_pageNum*m_booksPerPage);
            if (!meta)
                break;

            fe.setSize(12);
            fe.apply();
            pos.x = 0;
            pos.y = settings.smallSpace + fe.m_cur.ascender;
            fe.renderString(meta->title.c_str(), meta->title.length(), &pos, &clip, FE_XCLIP);
            pos.x = 0;
            pos.y = settings.smallSpace + fe.m_cur.ascender + fe.m_cur.lineHeight;

            fe.setSize(10);
            fe.apply();
            str.format("%u%% read   |   %s", meta->percentRead(), meta->fmtToStr(meta->format));
            fe.renderString(str.c_str(), str.length(), &pos, &clip, FE_XCLIP);

            m_bookRects[i] = clip;
            m_bookRects[i].h = pos.y + fe.m_cur.descender + settings.smallSpace;
            clip.y = m_bookRects[i].y + m_bookRects[i].h;

            g_fb->hline(clip.x, clip.y, clip.w);
        }

        drawChildren(m_rect.pos());
    }

    return drawn;
}


Activity LibraryActivity::run(UiBits& ui)
{
    clc::Log::info(LOG_NAME, "run");

    LibraryCanvas c(ui, g_library->m_meta);

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

