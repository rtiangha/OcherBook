/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/LibraryActivityFb.h"

#include "Container.h"
#include "resources/Bitmaps.h"
#include "shelf/Meta.h"
#include "util/Logger.h"
#include "util/StrUtil.h"
#include "ux/fb/FontEngine.h"
#include "ux/fb/UxControllerFb.h"
#include "ux/fb/Widgets.h"

#define LOG_NAME "ocher.ux.Library"


LibraryActivityFb::LibraryActivityFb(UxControllerFb *c) :
    ActivityFb(c),
    m_settings(g_container->settings),
    m_pageNum(0)
{
    // TODO calc this
    m_booksPerPage = BOOKS_PER_PAGE;
    m_bookRects = new Rect[m_booksPerPage];

    auto systemBar = make_unique<SystemBar>(m_uxController, g_container->battery);
    systemBar->m_sep = false;
    systemBar->setTitle("LIBRARY");
    m_systemBar = systemBar.get();
    addChild(std::move(systemBar));

    // TODO widget packing; button has padding so can go off screen
    auto leftIcon = make_unique<Button>(m_settings.medSpace + m_settings.largeSpace,
            m_rect.h - m_settings.medSpace*3 - bmpLeftArrow.h, bmpLeftArrow);
    leftIcon->pressed.Connect(this, &LibraryActivityFb::leftIconPressed);
    addChild(std::move(leftIcon));

    auto rightIcon = make_unique<Button>(m_rect.w - (m_settings.medSpace + m_settings.largeSpace + bmpRightArrow.w),
            m_rect.h - m_settings.medSpace*3 - bmpRightArrow.h, bmpRightArrow);
    rightIcon->pressed.Connect(this, &LibraryActivityFb::rightIconPressed);
    addChild(std::move(rightIcon));
}

LibraryActivityFb::~LibraryActivityFb()
{
    delete[] m_bookRects;
}

EventDisposition LibraryActivityFb::evtKey(const struct OcherKeyEvent *evt)
{
    if (evt->subtype == OEVT_KEY_DOWN) {
        if (evt->key == OEVTK_LEFT || evt->key == OEVTK_UP || evt->key == OEVTK_PAGEUP) {
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
    return ActivityFb::evtKey(evt);
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
    }
    return Widget::evtMouse(evt);
}

void LibraryActivityFb::drawContent(const Rect* rect)
{
    Log::debug(LOG_NAME, "draw");

    m_fb->setFg(0xff, 0xff, 0xff);
    m_fb->fillRect(rect);
    m_fb->setFg(0, 0, 0);

    Pos pos;
    Rect clip = m_fb->bbox;

    auto fe = m_uxController->getFontEngine();
    auto fc = fe->context();
    fc.setPoints(10);
    std::string str = format("PG. %u OF %u", m_pageNum + 1, m_pages);
    pos.x = 0;
    pos.y = clip.h - m_settings.smallSpace - fc.descender();
    fe->renderString(fc, str.c_str(), str.length(), &pos, &clip, FE_XCENTER);
// TODO        int maxY = pos.y - fc.ascender();

    clip.y = m_systemBar->rect().y + m_systemBar->rect().h + m_settings.medSpace;
    clip.x = m_settings.medSpace;
    clip.w -= m_settings.medSpace * 2;
    const std::vector<Meta *>& library = m_uxController->ctx.library.getList();
    for (unsigned int i = 0; i < m_booksPerPage; ++i) {
        size_t idx = i + m_pageNum * m_booksPerPage;
        if (idx >= library.size())
            break;
        Meta* meta = library[idx];

        fc.setPoints(12);
        pos.x = 0;
        pos.y = m_settings.smallSpace + fc.ascender();
        fe->renderString(fc, meta->title.c_str(), meta->title.length(), &pos, &clip, FE_XCLIP);
        pos.x = 0;
        pos.y = m_settings.smallSpace + fc.ascender() + fc.lineHeight();

        fc.setPoints(10);
        str = format("%u%% read   |   %s", meta->percentRead(), meta->fmtToStr(meta->format));
        fe->renderString(fc, str.c_str(), str.length(), &pos, &clip, FE_XCLIP);

        m_bookRects[i] = clip;
        m_bookRects[i].h = pos.y + fc.descender() + m_settings.smallSpace;
        clip.y = m_bookRects[i].y + m_bookRects[i].h;

        m_fb->hline(clip.x, clip.y, clip.w);
    }
}

void LibraryActivityFb::onAttached()
{
    Log::info(LOG_NAME, "attached");

    const std::vector<Meta *>& library = m_uxController->ctx.library.getList();
    m_pages = (library.size() + m_booksPerPage - 1) / m_booksPerPage;
    Log::info(LOG_NAME, "%u books across %u pages", (unsigned)library.size(), m_pages);
}

void LibraryActivityFb::leftIconPressed(Button&)
{
    Log::info(LOG_NAME, "Left icon pressed");
    if (m_pageNum > 0) {
        m_pageNum--;
        invalidate();
    }
}

void LibraryActivityFb::rightIconPressed(Button&)
{
    Log::info(LOG_NAME, "Right icon pressed");
    if (m_pageNum + 1 < m_pages) {
        m_pageNum++;
        invalidate();
    }
}
