/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/LibraryActivityFb.h"

#include "Container.h"
#include "resources/Bitmaps.h"
#include "settings/Settings.h"
#include "shelf/Meta.h"
#include "util/Logger.h"
#include "util/StrUtil.h"
#include "ux/fb/FontEngine.h"
#include "ux/fb/UxControllerFb.h"
#include "ux/fb/Widgets.h"

#define LOG_NAME "ocher.ux.Library"


LibraryActivityFb::LibraryActivityFb(UxControllerFb *c) :
    ActivityFb(c),
    m_systemBar(c->m_systemBar),
    m_settings(g_container.settings),
    m_pageNum(0)
{
    maximize();

    // TODO calc this
    m_booksPerPage = BOOKS_PER_PAGE;
    m_bookRects = new Rect[m_booksPerPage];

    m_systemBar->m_sep = false;
    m_systemBar->m_title = "LIBRARY";
    m_systemBar->show();
    addChild(m_systemBar);
    addChild(new Icon(m_settings->medSpace + m_settings->largeSpace,
                    m_rect.h - m_settings->medSpace - bmpLeftArrow.h, &bmpLeftArrow));
    addChild(new Icon(m_rect.w - (m_settings->medSpace + m_settings->largeSpace + bmpRightArrow.w),
                    m_rect.h - m_settings->medSpace - bmpRightArrow.h, &bmpRightArrow));

}

LibraryActivityFb::~LibraryActivityFb()
{
    delete[] m_bookRects;
}

EventDisposition LibraryActivityFb::evtKey(const struct OcherKeyEvent *evt)
{
    if (evt->subtype == OEVT_KEY_DOWN) {
        if (evt->key == OEVTK_HOME) {
            Log::info(LOG_NAME, "home");
            m_uxController->setNextActivity(Activity::Type::Home);
            return EventDisposition::Handled;
        } else if (evt->key == OEVTK_LEFT || evt->key == OEVTK_UP || evt->key == OEVTK_PAGEUP) {
            Log::info(LOG_NAME, "back from page %u", m_pageNum);
            if (m_pageNum > 0) {
                m_pageNum--;
                invalidate();
            }
            return EventDisposition::Handled;
        } else if (evt->key == OEVTK_RIGHT || evt->key == OEVTK_DOWN || evt->key == OEVTK_PAGEDOWN) {
            Log::info(LOG_NAME, "forward from page %u", m_pageNum);
            if (m_pageNum + 1 < m_pages) {
                m_pageNum++;
                invalidate();
            }
            return EventDisposition::Handled;
        }
    }
    return EventDisposition::Pass;
}

EventDisposition LibraryActivityFb::evtMouse(const struct OcherMouseEvent *evt)
{
    if (evt->subtype == OEVT_MOUSE1_UP) {
        Pos pos(evt->x, evt->y);
        const std::vector<Meta *>& library = m_uxController->ctx.library.getList();
        for (unsigned int i = 0; i < m_booksPerPage; i++) {
            size_t idx = i + m_pageNum * m_booksPerPage;
            if (idx >= library.size())
                break;
            Meta* meta = library[idx];
            if (m_bookRects[i].contains(pos)) {
                m_uxController->ctx.selected = meta;
                m_uxController->setNextActivity(Activity::Type::Read);
                return EventDisposition::Handled;
            }
        }
        // TODO buttons
        if (pos.x < 300) {
            Log::info(LOG_NAME, "back from page %d", m_pageNum);
            if (m_pageNum > 0) {
                m_pageNum--;
                invalidate();
            }
            return EventDisposition::Handled;
        } else {
            Log::info(LOG_NAME, "forward from page %d", m_pageNum);
            if (m_pageNum + 1 < m_pages) {
                m_pageNum++;
                invalidate();
            }
            return EventDisposition::Handled;
        }
    }
    return EventDisposition::Pass;
}

void LibraryActivityFb::draw()
{
    Log::debug(LOG_NAME, "draw");

    FrameBuffer* fb = m_screen->fb;
    fb->setFg(0xff, 0xff, 0xff);
    fb->fillRect(&m_rect);
    fb->setFg(0, 0, 0);

    FontEngine fe(fb);
    Pos pos;
    Rect clip = fb->bbox;

    fe.setSize(10);
    fe.apply();
    std::string str = format("PG. %u OF %u", m_pageNum + 1, m_pages);
    pos.x = 0;
    pos.y = clip.h - m_settings->smallSpace - fe.m_cur.descender;
    fe.renderString(str.c_str(), str.length(), &pos, &clip, FE_XCENTER);
// TODO        int maxY = pos.y - fe.m_cur.ascender;

    clip.y = m_systemBar->rect().y + m_systemBar->rect().h + m_settings->medSpace;
    clip.x = m_settings->medSpace;
    clip.w -= m_settings->medSpace * 2;
    const std::vector<Meta *>& library = m_uxController->ctx.library.getList();
    for (unsigned int i = 0; i < m_booksPerPage; ++i) {
        size_t idx = i + m_pageNum * m_booksPerPage;
        if (idx >= library.size())
            break;
        Meta* meta = library[idx];

        fe.setSize(12);
        fe.apply();
        pos.x = 0;
        pos.y = m_settings->smallSpace + fe.m_cur.ascender;
        fe.renderString(meta->title.c_str(), meta->title.length(), &pos, &clip, FE_XCLIP);
        pos.x = 0;
        pos.y = m_settings->smallSpace + fe.m_cur.ascender + fe.m_cur.lineHeight;

        fe.setSize(10);
        fe.apply();
        str = format("%u%% read   |   %s", meta->percentRead(), meta->fmtToStr(meta->format));
        fe.renderString(str.c_str(), str.length(), &pos, &clip, FE_XCLIP);

        m_bookRects[i] = clip;
        m_bookRects[i].h = pos.y + fe.m_cur.descender + m_settings->smallSpace;
        clip.y = m_bookRects[i].y + m_bookRects[i].h;

        fb->hline(clip.x, clip.y, clip.w);
    }
}

void LibraryActivityFb::onAttached()
{
    Log::info(LOG_NAME, "attached");

    const std::vector<Meta *>& library = m_uxController->ctx.library.getList();
    m_pages = (library.size() + m_booksPerPage - 1) / m_booksPerPage;
    Log::info(LOG_NAME, "%u books across %u pages", (unsigned)library.size(), m_pages);

    m_systemBar->m_sep = false;
    m_systemBar->m_title.clear();
    m_systemBar->show();

    invalidate();
}

void LibraryActivityFb::onDetached()
{
    Log::info(LOG_NAME, "detached");
}
