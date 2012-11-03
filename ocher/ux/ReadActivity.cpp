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
#include "ocher/ux/Factory.h"
#include "ocher/shelf/Meta.h"
#include "ocher/ux/Controller.h"
#include "ocher/ux/ReadActivity.h"


ReadActivity::ReadActivity()
{
}

void read(Meta* meta)
{
    Renderer* renderer = uiFactory->getRenderer();

    int& pageNum = meta->record.lastPage;
    while (1) {
        int atEnd = renderer->render(&meta->m_pagination, pageNum, true);

        struct OcherEvent evt;
        int oldPageNum = pageNum;
        while (pageNum == oldPageNum) {
            if (g_loop->wait(&evt) == 0) {
                switch (evt.type) {
                    case OEVT_KEY:
                        if (evt.subtype == OEVT_KEY_DOWN) {
                            if (evt.key.key == OEVTK_HOME) {
                                return;
                            } else if (evt.key.key == OEVTK_LEFT || evt.key.key == OEVTK_UP || evt.key.key == OEVTK_PAGEUP) {
                                if (pageNum > 0)
                                    pageNum--;
                            } else if (evt.key.key == OEVTK_RIGHT || evt.key.key == OEVTK_DOWN || evt.key.key == OEVTK_PAGEDOWN) {
                                if (! atEnd)
                                    pageNum++;
                            }
                        }
                        break;
                    case OEVT_MOUSE:
                        if (evt.subtype == OEVT_MOUSE1_CLICKED || evt.subtype == OEVT_MOUSE1_DOWN) {
                            if (evt.mouse.y > g_fb->height()*.9) {
                                // TODO
                            } else if (evt.mouse.x < g_fb->width()/2) {
                                if (pageNum > 0)
                                    pageNum--;
                            } else {
                                if (!atEnd)
                                    pageNum++;
                            }
                            break;
                        }
                    case OEVT_APP:
                        if (evt.subtype == OEVT_APP_CLOSE)
                            exit(0);
                }
            }
        }
    }
}

Activity ReadActivity::run()
{
    Meta* meta = (Meta*)g_cont->m_meta.get(0);
    if (!meta)
        return ACTIVITY_HOME;

    clc::Buffer memLayout;
    // TODO:  rework Layout constructors to have separate init due to scoping
    Layout *layout = 0;
    const char* file = meta->relPath.c_str();
    clc::Log::info("ocher", "Loading %s: %s", Meta::fmtToStr(meta->format), file);
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
                mxml_node_t *tree = epub.parseXml(html);
                if (tree) {
                    ((LayoutEpub*)layout)->append(tree);
                    mxmlDelete(tree);
                } else {
                    clc::Log::warn("ocher", "No tree found for spine item %d", i);
                }
            }
            memLayout = layout->unlock();
            break;
        }
#endif
        default: {
            clc::Log::warn("ocher", "Unhandled format %d", meta->format);
        }
    }

    Renderer* renderer = uiFactory->getRenderer();
    renderer->set(memLayout);

    // Optionally, run through all pages without blitting to get an accurate
    // page count.
#if 0
    for (int pageNum = 0; ; pageNum++) {
        clc::Log::info("ocher", "Paginating page %d", pageNum);
        int r = renderer->render(&meta->m_pagination, pageNum, false);

        if (r != 0)
            break;
    }
#endif

    read(meta);

    delete layout;

    return ACTIVITY_HOME;
}
