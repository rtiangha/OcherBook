#include "clc/support/Logger.h"

#include "ocher_config.h"
#ifdef OCHER_EPUB
#include "ocher/fmt/epub/Epub.h"
#include "ocher/fmt/epub/LayoutEpub.h"
#endif
#if 1
#include "ocher/fmt/text/LayoutText.h"
#include "ocher/fmt/text/Text.h"
#endif
#include "ocher/shelf/Meta.h"
#include "ocher/ux/Controller.h"
#include "ocher/ux/Factory.h"
#include "ocher/ux/ReadActivity.h"

#define LOG_NAME "ocher.ux.Read"


int ReadActivity::evtKey(struct OcherEvent* evt)
{
    if (evt->subtype == OEVT_KEY_DOWN) {
        if (evt->key.key == OEVTK_HOME) {
            m_next = ACTIVITY_HOME;
            return 0;
        } else if (evt->key.key == OEVTK_LEFT || evt->key.key == OEVTK_UP || evt->key.key == OEVTK_PAGEUP) {
            clc::Log::debug(LOG_NAME, "back from page %d", m_pageNum);
            if (m_pageNum > 0) {
                m_pageNum--;
                dirty();
            }
        } else if (evt->key.key == OEVTK_RIGHT || evt->key.key == OEVTK_DOWN || evt->key.key == OEVTK_PAGEDOWN) {
            clc::Log::debug(LOG_NAME, "forward from page %d", m_pageNum);
            if (! atEnd) {
                m_pageNum++;
                dirty();
            }
        }
        return -1;
    }
    return -2;
}

int ReadActivity::evtMouse(struct OcherEvent* evt)
{
    if (evt->subtype == OEVT_MOUSE1_CLICKED || evt->subtype == OEVT_MOUSE1_DOWN) {
        int sby = m_ui.m_systemBar.m_rect.h;
        int sbh = m_ui.m_systemBar.m_flags & WIDGET_HIDDEN;
        if (evt->mouse.y <= sby && !sbh) {
            clc::Log::debug(LOG_NAME, "interact system bar");
            // TODO interact
        } else if (evt->mouse.y <= sby && sbh) {
            clc::Log::debug(LOG_NAME, "show system bar");
            m_ui.m_systemBar.show();
            dirty();
        } else if (evt->mouse.y > sby && !sbh) {
            clc::Log::debug(LOG_NAME, "hide system bar");
            m_ui.m_systemBar.hide();
            dirty();
        } else if (evt->mouse.x < g_fb->width()/2) {
            if (m_pageNum > 0) {
                m_pageNum--;
                dirty();
            }
        } else {
            if (!atEnd) {
                m_pageNum++;
                dirty();
            }
        }
        return -1;
    }
    return -2;
}

ReadActivity::ReadActivity(UiBits& ui) :
    m_ui(ui)
{
}

Rect ReadActivity::draw(Pos*)
{
    Rect drawn;
    drawn.setInvalid();

    if (m_flags & WIDGET_DIRTY) {
        m_flags &= ~WIDGET_DIRTY;
        drawn = m_rect;
        atEnd = renderer->render(&meta->m_pagination, m_pageNum, true);
    }
    Rect d2 = drawChildren(m_rect.pos());
    drawn.unionRect(&d2);
    return drawn;
}

Activity ReadActivity::run()
{
    clc::Log::debug(LOG_NAME, "run");
    meta = g_shelf->selected();
    if (!meta) {
        clc::Log::error(LOG_NAME, "No book selected");
        return ACTIVITY_HOME;
    }
    clc::Log::debug(LOG_NAME, "selected %p", meta);
    g_shelf->markActive(meta);

    // TODO:  rework Layout constructors to have separate init due to scoping
    Layout* layout = 0;
    clc::Buffer memLayout;
    const char* file = meta->relPath.c_str();
    clc::Log::info(LOG_NAME, "Loading %s: %s", Meta::fmtToStr(meta->format), file);
    switch (meta->format) {
        case OCHER_FMT_TEXT: {
            Text text(file);
            layout = new LayoutText(&text);
            memLayout = layout->unlock();
            break;
        }
#ifdef OCHER_EPUB
        case OCHER_FMT_EPUB: {
            Epub epub(file);
            layout = new LayoutEpub(&epub);
            clc::Buffer html;
            for (int i = 0; ; i++) {
                if (epub.getSpineItemByIndex(i, html) != 0)
                    break;
                mxml_node_t* tree = epub.parseXml(html);
                if (tree) {
                    ((LayoutEpub*)layout)->append(tree);
                    mxmlDelete(tree);
                } else {
                    clc::Log::warn(LOG_NAME, "No tree found for spine item %d", i);
                }
            }
            memLayout = layout->unlock();
            break;
        }
#endif
        default: {
            clc::Log::warn(LOG_NAME, "Unhandled format %d", meta->format);
        }
    }

    renderer = uiFactory->getRenderer();
    renderer->set(memLayout);

    // Optionally, run through all pages without blitting to get an accurate
    // page count.
#if 0
    for (int m_pageNum = 0; ; m_pageNum++) {
        clc::Log::info(LOG_NAME, "Paginating page %d", m_pageNum);
        int r = renderer->render(&meta->m_pagination, m_pageNum, false);

        if (r != 0)
            break;
    }
#endif

    m_ui.m_systemBar.hide();
    addChild(m_ui.m_systemBar);
    m_pageNum = 0;
    while (1) {
        refresh();

        // TODO  convert to g_loop->poll(this);
        struct OcherEvent evt;
        if (g_loop->wait(&evt) == 0) {
            int r = eventReceived(&evt);
            if (r >= 0)
                break;
        }
    }

    delete layout;

    return m_next;
}
