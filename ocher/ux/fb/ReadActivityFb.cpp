/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher_config.h"

#include "ux/fb/ReadActivityFb.h"

#include "Container.h"
#include "fmt/text/LayoutText.h"
#include "fmt/text/Text.h"
#include "settings/Settings.h"
#include "shelf/Meta.h"
#include "util/Debug.h"
#include "util/Logger.h"
#include "ux/Renderer.h"
#include "ux/fb/UxControllerFb.h"
#ifdef FMT_EPUB
#include "fmt/epub/Epub.h"
#include "fmt/epub/LayoutEpub.h"
#endif

#define LOG_NAME "ocher.ux.Read"


EventDisposition ReadActivityFb::evtKey(const struct OcherKeyEvent* evt)
{
    if (evt->subtype == OEVT_KEY_DOWN) {
        if (evt->key == OEVTK_HOME) {
            Log::info(LOG_NAME, "home");
            // TODO  visually turn page down
            m_uxController->setNextActivity(Activity::Type::Home);
            return EventDisposition::Handled;
        } else if (evt->key == OEVTK_LEFT || evt->key == OEVTK_UP || evt->key == OEVTK_PAGEUP) {
            m_systemBar->hide();
            m_navBar->hide();
            turnPages(-1);
            return EventDisposition::Handled;
        } else if (evt->key == OEVTK_RIGHT || evt->key == OEVTK_DOWN || evt->key == OEVTK_PAGEDOWN) {
            m_systemBar->hide();
            m_navBar->hide();
            turnPages(1);
            return EventDisposition::Handled;
        }
    }
    return ActivityFb::evtKey(evt);
}

EventDisposition ReadActivityFb::evtMouse(const struct OcherMouseEvent* evt)
{
    if (evt->subtype == OEVT_MOUSE1_UP) {
        Pos pos(evt->x, evt->y);
        if (m_systemBar->rect().contains(pos) || m_navBar->rect().contains(pos)) {
            if (m_systemBar->m_flags & WIDGET_HIDDEN) {
                Log::info(LOG_NAME, "show system bar");
                m_systemBar->show();
                m_navBar->show();
            } else {
                return Widget::evtMouse(evt);
            }
        } else {
            if (!(m_systemBar->m_flags & WIDGET_HIDDEN)) {
                Log::info(LOG_NAME, "hide system bar");
                m_systemBar->hide();
                m_navBar->hide();
            } else {
                if (evt->x < m_fb->xres() / 2) {
                    turnPages(-1);
                } else {
                    turnPages(1);
                }
            }
        }
        return EventDisposition::Handled;
    }
    return Widget::evtMouse(evt);
}

ReadActivityFb::ReadActivityFb(UxControllerFb* c) :
    ActivityFb(c),
    m_fb(c->getFrameBuffer()),
    atEnd(1),
    m_pagesSinceRefresh(0)
{
    auto systemBar = make_unique<SystemBar>(m_uxController, g_container->battery);
    systemBar->m_sep = true;
    systemBar->hide();
    m_systemBar = systemBar.get();
    addChild(std::move(systemBar));

    auto navBar = make_unique<NavBar>();
    m_navBar = navBar.get();
    addChild(std::move(navBar));

    m_navBar->backButton().pressed.Connect(this, &ReadActivityFb::backButtonPressed);
    m_navBar->forwardButton().pressed.Connect(this, &ReadActivityFb::forwardButtonPressed);

    maximize();
}

ReadActivityFb::~ReadActivityFb()
{
    m_navBar->backButton().pressed.Disconnect(this, &ReadActivityFb::backButtonPressed);
    m_navBar->forwardButton().pressed.Disconnect(this, &ReadActivityFb::forwardButtonPressed);
}

void ReadActivityFb::turnPages(int n)
{
    if (n < 0) {
        n = -n;
        if (n >= m_pageNum)
            n = m_pageNum;
        m_pageNum -= n;
        Log::info(LOG_NAME, "back from page %d to %d", m_pageNum + n, m_pageNum);
        invalidate();
    } else if (n > 0) {
        if (n > meta->pages - m_pageNum - 1)
            n = meta->pages - m_pageNum - 1;
        m_pageNum += n;
        Log::info(LOG_NAME, "forward from page %d to %d", m_pageNum - n, m_pageNum);
        invalidate();
    }
}

void ReadActivityFb::backButtonPressed(Button&)
{
    turnPages(-10);
}

void ReadActivityFb::forwardButtonPressed(Button&)
{
    turnPages(10);
}

void ReadActivityFb::drawContent(const Rect* rect)
{
    Log::debug(LOG_NAME, "draw");

    m_pagesSinceRefresh++;
    if (m_pagesSinceRefresh >= g_container->settings.fullRefreshPages) {
        m_pagesSinceRefresh = 0;
        m_fb->needFull();
    }
    atEnd = m_renderer->render(&meta->m_pagination, m_pageNum, true);
}

void ReadActivityFb::onAttached()
{
    Log::info(LOG_NAME, "attached");

    m_renderer = m_uxController->getRenderer();
    meta = m_uxController->ctx.selected;
    ASSERT(meta);
    Log::debug(LOG_NAME, "selected %p", meta);

    m_systemBar->setTitle(meta->title);

    m_fb->clear();

    const char* file = meta->relPath.c_str();
    Log::info(LOG_NAME, "Loading %s: %s", Meta::fmtToStr(meta->format), file);
    switch (meta->format) {
    case OCHER_FMT_TEXT: {
        Text text(file);
        LayoutText layout(&text);
        m_renderer->set(layout.finish());
        break;
    }
#ifdef FMT_EPUB
    case OCHER_FMT_EPUB: {
        Epub epub(file);
        LayoutEpub layout(&epub);
        std::string html;
        for (int i = 0;; i++) {
            if (epub.getSpineItemByIndex(i, html) != 0)
                break;
            mxml_node_t* tree = epub.parseXml(html);
            if (tree) {
                layout.append(tree);
                mxmlDelete(tree);
            } else {
                Log::warn(LOG_NAME, "No tree found for spine item %d", i);
            }
        }
        m_renderer->set(layout.finish());
        break;
    }
#endif
    default: {
        Log::warn(LOG_NAME, "Unhandled format %d", meta->format);
    }
    }

    // Optionally, run through all pages without blitting to get an accurate
    // page count.  Alternative is to do some sort of "idealize" page layout that might be faster.
#if 1
    if (meta->m_pagination.numPages() == 0) {
        for (int pageNum = 0;; pageNum++) {
            Log::info(LOG_NAME, "Paginating page %d", pageNum);
            int r = m_renderer->render(&meta->m_pagination, pageNum, false);

            if (r != 0) {
                meta->pages = pageNum + 1;
                break;
            }
        }
    }
#endif

    m_navBar->hide();

    meta->record.touch();
    m_pageNum = meta->record.activePage;
    Log::info(LOG_NAME, "Starting on page %u", m_pageNum);
}

void ReadActivityFb::onDetached()
{
    Log::info(LOG_NAME, "Quitting on page %u", m_pageNum);

    meta->record.activePage = m_pageNum;
}
