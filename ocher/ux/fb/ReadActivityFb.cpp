/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher/ux/fb/ReadActivityFb.h"

#ifdef OCHER_EPUB
#include "ocher/fmt/epub/Epub.h"
#include "ocher/fmt/epub/LayoutEpub.h"
#endif
#if 1
#include "ocher/fmt/text/LayoutText.h"
#include "ocher/fmt/text/Text.h"
#endif
#include "ocher/Container.h"
#include "ocher/settings/Settings.h"
#include "ocher/shelf/Meta.h"
#include "ocher/ux/Renderer.h"
#include "ocher/ux/fb/UxControllerFb.h"
#include "ocher/util/Debug.h"
#include "ocher/util/Logger.h"

#define LOG_NAME "ocher.ux.Read"


int ReadActivityFb::evtKey(const struct OcherKeyEvent* evt)
{
    if (evt->subtype == OEVT_KEY_DOWN) {
        if (evt->key == OEVTK_HOME) {
            Log::info(LOG_NAME, "home");
            // TODO  visually turn page down
            m_uxController->setNextActivity(ACTIVITY_HOME);
            return -1;
        } else if (evt->key == OEVTK_LEFT || evt->key == OEVTK_UP || evt->key == OEVTK_PAGEUP) {
            Log::info(LOG_NAME, "back from page %d", m_pageNum);
            if (m_pageNum > 0) {
                m_pageNum--;
                m_uxController->m_systemBar->hide();
                m_uxController->m_navBar->hide();
                invalidate();
            }
            return -1;
        } else if (evt->key == OEVTK_RIGHT || evt->key == OEVTK_DOWN || evt->key == OEVTK_PAGEDOWN) {
            Log::info(LOG_NAME, "forward from page %d", m_pageNum);
            if (!atEnd) {
                m_pageNum++;
                m_uxController->m_systemBar->hide();
                m_uxController->m_navBar->hide();
                invalidate();
            }
            return -1;
        }
    }
    return -2;
}

int ReadActivityFb::evtMouse(const struct OcherMouseEvent* evt)
{
    SystemBar* systemBar = m_uxController->m_systemBar;
    NavBar* navBar = m_uxController->m_navBar;

    if (evt->subtype == OEVT_MOUSE1_UP) {
        Pos pos(evt->x, evt->y);
        if (systemBar->m_rect.contains(&pos) || navBar->m_rect.contains(&pos)) {
            if (systemBar->m_flags & WIDGET_HIDDEN) {
                Log::info(LOG_NAME, "show system bar");
                systemBar->show();
                m_fb->update(&systemBar->m_rect);
                navBar->show();
                m_fb->update(&navBar->m_rect);
            } else {
                Log::info(LOG_NAME, "interact bar");
                // TODO interact
            }
        } else {
            if (!(systemBar->m_flags & WIDGET_HIDDEN)) {
                Log::info(LOG_NAME, "hide system bar");
                systemBar->hide();
                navBar->hide();
                invalidate();
            } else {
                if (evt->x < m_fb->width() / 2) {
                    if (m_pageNum > 0) {
                        Log::info(LOG_NAME, "back from page %d", m_pageNum);
                        m_pageNum--;
                        invalidate();
                    }
                } else {
                    if (!atEnd) {
                        Log::info(LOG_NAME, "forward from page %d", m_pageNum);
                        m_pageNum++;
                        invalidate();
                    }
                }
            }
        }
        return -1;
    }
    return -2;
}

ReadActivityFb::ReadActivityFb(UxControllerFb* c) :
    ActivityFb(c),
    m_fb(c->getFrameBuffer()),
    m_settings(g_container.settings),
    m_layout(nullptr),
    atEnd(1),
    m_pagesSinceRefresh(0)
{
    maximize();
}

void ReadActivityFb::draw()
{
    Log::debug(LOG_NAME, "draw");

    m_pagesSinceRefresh++;
    if (m_pagesSinceRefresh >= m_settings->fullRefreshPages) {
        m_pagesSinceRefresh = 0;
        m_fb->needFull();
    }
    atEnd = m_renderer->render(&meta->m_pagination, m_pageNum, true);
}

void ReadActivityFb::onAttached()
{
    Log::info(LOG_NAME, "attached");

    meta = m_uxController->ctx.selected;
    ASSERT(meta);
    Log::debug(LOG_NAME, "selected %p", meta);

    m_fb->clear();
    m_fb->update(nullptr);

    ASSERT(m_layout == nullptr);
    Buffer memLayout;
    const char* file = meta->relPath.c_str();
    Log::info(LOG_NAME, "Loading %s: %s", Meta::fmtToStr(meta->format), file);
    switch (meta->format) {
    case OCHER_FMT_TEXT: {
        Text text(file);
        m_layout = new LayoutText(&text);
        memLayout = m_layout->unlock();
        break;
    }
#ifdef OCHER_EPUB
    case OCHER_FMT_EPUB: {
        Epub epub(file);
        m_layout = new LayoutEpub(&epub);
        std::string html;
        for (int i = 0;; i++) {
            if (epub.getSpineItemByIndex(i, html) != 0)
                break;
#if 1
            mxml_node_t* tree = epub.parseXml(html);
            if (tree) {
                static_cast<LayoutEpub*>(m_layout)->append(tree);
                mxmlDelete(tree);
            } else {
                Log::warn(LOG_NAME, "No tree found for spine item %d", i);
            }
#else
            ((LayoutEpub*)m_layout)->append(html);
#endif
        }
        memLayout = m_layout->unlock();
        break;
    }
#endif
    default: {
        Log::warn(LOG_NAME, "Unhandled format %d", meta->format);
    }
    }

    m_renderer = g_container.renderer;
    m_renderer->set(memLayout);

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

    SystemBar* systemBar = m_uxController->m_systemBar;
    NavBar* navBar = m_uxController->m_navBar;

    addChild(systemBar);
    systemBar->m_sep = true;
    systemBar->m_title = meta->title;
    systemBar->hide();

    navBar->hide();
    addChild(navBar);

    meta->record.touch();
    m_pageNum = meta->record.activePage;
    Log::info(LOG_NAME, "Starting on page %u", m_pageNum);
    invalidate();
}

void ReadActivityFb::onDetached()
{
    Log::info(LOG_NAME, "Quitting on page %u", m_pageNum);

    meta->record.activePage = m_pageNum;

    removeChild(m_uxController->m_systemBar);
    removeChild(m_uxController->m_navBar);

    if (m_layout) {
        delete m_layout;
        m_layout = nullptr;
    }
}
